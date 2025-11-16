#include "Structures.hlsl"

#ifndef PRIMARY_X_TS_THR_COUNT
    #define PRIMARY_X_TS_THR_COUNT 32
#endif

#ifndef PRIMARY_Y_TS_THR_COUNT
    #define PRIMARY_Y_TS_THR_COUNT 1
#endif

// Objects scales, rotations and positions
// Set 0
StructuredBuffer<Meshlet> Meshlets : register(t2, space0);
//StructuredBuffer<BoundingBox> MeshBoxes : register(t3, space0); // unused
StructuredBuffer<DrawMeshData> DrawData : register(t4, space0);
// Set 1
cbuffer CamFrustumData : register(b0, space1)
{
    Frustum Frustum;
}

cbuffer CamData : register(b1, space1)
{
    CameraData CameraData;
}
// Set 2
StructuredBuffer<MeshMatrix> MeshMatrices : register(t0, space2);

// 16 bytes
struct Constants
{
    uint InstanceCount;
    uint CurrentMeshID;
    uint Padding[2];
};

[[vk::push_constant]]
Constants constants;


struct Payload
{
    uint MeshletIndices[PRIMARY_X_TS_THR_COUNT];
    uint InstanceIndices[PRIMARY_X_TS_THR_COUNT];
};

// Payloads
groupshared Payload gs_Payload;
groupshared uint gs_VisibleCount;

inline float GetSignedDistanceToPlane(float4 plane, float3 point_)
{
    return dot(plane.xyz, point_) - plane.w;
}

inline bool IsOnOrForwardPlane(float4 plane, BoundingSphere sphere)
{
    return GetSignedDistanceToPlane(plane, sphere.Center) >= -sphere.Center;
}

bool CullMeshlet(uint meshletIndex, uint instanceIndex)
{
    BoundingSphere sphere = Meshlets[meshletIndex].Sphere;
    sphere.Center = mul(float4(sphere.Center, 1.f), MeshMatrices[instanceIndex].Matrix).xyz;
    sphere.Radius *= max(
        max(length(MeshMatrices[instanceIndex].Matrix[0].xyz),
            length(MeshMatrices[instanceIndex].Matrix[1].xyz)), 
        length(MeshMatrices[instanceIndex].Matrix[2].xyz)
    );

    return  IsOnOrForwardPlane(Frustum.Planes[0], sphere) &&
            IsOnOrForwardPlane(Frustum.Planes[1], sphere) &&
            IsOnOrForwardPlane(Frustum.Planes[2], sphere) &&
            IsOnOrForwardPlane(Frustum.Planes[3], sphere) &&
            IsOnOrForwardPlane(Frustum.Planes[4], sphere) &&
            IsOnOrForwardPlane(Frustum.Planes[5], sphere);
}

[numthreads(PRIMARY_X_TS_THR_COUNT, 1, 1)]
void TSMain(uint3 threadID              : SV_GroupThreadID,
            uint3 dispatchThreadID      : SV_DispatchThreadID, 
            uint3 groupID               : SV_GroupID)
{
    DrawMeshData drawData = DrawData[constants.CurrentMeshID];

    uint instanceID = dispatchThreadID.x / drawData.MeshletCount;
    uint meshletID = dispatchThreadID.x % drawData.MeshletCount;

    if (instanceID >= constants.InstanceCount)
        return;

    if (threadID.x == 0)
        gs_VisibleCount = 0;

    //bool visible = CullMeshlet(meshletID, instanceID);
    bool visible = true;
    if (visible)
    {
        uint index;
        InterlockedAdd(gs_VisibleCount, 1, index);
        if (index < PRIMARY_X_TS_THR_COUNT)
        {
            gs_Payload.InstanceIndices[index] = instanceID;
            gs_Payload.MeshletIndices[index] = meshletID;
        }
    }

    GroupMemoryBarrierWithGroupSync();

    if (threadID.x == 0)
    {
        uint visibleCount = gs_VisibleCount;
        DispatchMesh(visibleCount, 1, 1, gs_Payload);
    }
}