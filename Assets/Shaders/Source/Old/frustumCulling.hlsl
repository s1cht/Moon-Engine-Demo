#include "Structures.hlsl"

//RWStructuredBuffer<DebugMsgData> DebugMessages : register(b0, space0);

StructuredBuffer<BoundingBox> MeshBoxBuffer                         : register(t0, space0);
StructuredBuffer<IndirectDrawDataAndMeshData> InputIndirectDraws    : register(t0, space1);

cbuffer FrustumBuffer                                               : register(b0, space2)
{
    Frustum frustum;
}

RWStructuredBuffer<IndirectDrawDataAndMeshData> OutputIndirectDraws : register(u0, space3);
RWBuffer<uint> OutputIndirectCountBuffer                            : register(u1, space3);

struct Constants
{
    uint count;
};

[[vk::push_constant]]
Constants constants;

void cullMesh(uint meshId)
{
    BoundingBox meshBox = MeshBoxBuffer[meshId];
    bool isVisible = true;

    for (int i = 0; i < 6 && isVisible; i++)
    {
        float3 planeNormal = frustum.planes[i].xyz;
        float distanceFromPlane = dot(meshBox.meshCenterPosition.xyz, planeNormal);

        float absDiff = dot(abs(planeNormal), meshBox.extents.xyz);
        if (distanceFromPlane + absDiff + frustum.planes[i].w < 0.f)
            isVisible = false;
    }

    if (isVisible)
    {
        uint index;
        InterlockedAdd(OutputIndirectCountBuffer[0], 1, index);
        OutputIndirectDraws[index] = InputIndirectDraws[meshId];
    }
}

[numthreads(256, 1, 1)]
void CSMain(uint3 id : SV_DispatchThreadID)
{
    uint oldValue;
    if (id.x == 0)
        InterlockedExchange(OutputIndirectCountBuffer[0], 0, oldValue);

    AllMemoryBarrier();

    if (id.x < constants.count)
    {
        cullMesh(id.x);
    }
}