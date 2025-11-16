#include "Structures.hlsl"
#include "Globals.hlsl"

#ifndef PRIMARY_X_TS_THR_COUNT
    #define PRIMARY_X_TS_THR_COUNT 32
#endif

#ifndef PRIMARY_Y_TS_THR_COUNT
    #define PRIMARY_Y_TS_THR_COUNT 1
#endif

#ifndef PRIMARY_X_MS_THR_COUNT
    #define PRIMARY_X_MS_THR_COUNT 64
#endif

// Buffers
// Set 0
StructuredBuffer<Vertex> Vertices : register(t0, space0);
StructuredBuffer<uint> Indices : register(t1, space0);
StructuredBuffer<Meshlet> Meshlets : register(t2, space0);
StructuredBuffer<DrawMeshData> DrawData : register(t4, space0);
// Set 1
cbuffer CamData : register(b1, space1)
{
    CameraData CameraData;
}
// Set 2
StructuredBuffer<MeshMatrix> MeshMatrices : register(t0, space2);

struct MeshOutputVertex
{
    float4 Position : SV_Position;
    float3 Normal : NORMAL0;
    float2 TexCoord : TEXCOORD0;
    uint MeshID : COLOR0;
};

// Row-Major
float4 CalculatePosition(float3 position, float4x4 objectTransform)
{
    return mul(mul(mul(float4(position, 1.f), objectTransform), CameraData.ViewMatrix), CameraData.ProjectionMatrix);
}

inline float4 CalculatePositionCM(float3 position, float4x4 objectTransform)
{
    return mul(mul(mul(CameraData.ProjectionMatrix, CameraData.ViewMatrix), objectTransform), float4(position, 1.f));
}

struct Payload
{
    uint MeshletIndices[PRIMARY_X_TS_THR_COUNT];
    uint InstanceIndices[PRIMARY_X_TS_THR_COUNT];
};

// Shared memory for surviving meshlets
groupshared uint visibleCount[PRIMARY_Y_TS_THR_COUNT];
groupshared bool instanceVisible[PRIMARY_Y_TS_THR_COUNT];

[NumThreads(PRIMARY_X_MS_THR_COUNT, 1, 1)]
[OutputTopology("triangle")]
void MSMain(
    uint3 groupID   : SV_GroupID,
    uint3 threadID  : SV_GroupThreadID,
    in  payload     Payload             inPayload,
    out vertices    MeshOutputVertex    outVertices[MS_MAX_VERTS],
    out indices     uint3               outIndices[MS_MAX_TRIANGLES]
)
{
    uint instanceId = inPayload.InstanceIndices[groupID.x];
    uint meshletId = inPayload.MeshletIndices[groupID.x];
    Meshlet meshlet = Meshlets[meshletId];
    float4x4 transform = MeshMatrices[instanceId].Matrix;

    // Set output counts
    SetMeshOutputCounts(meshlet.VertexCount, meshlet.TriangleCount);

    // Process vertices
    if (threadID.x < meshlet.VertexCount)
    {
        uint vertexIndex = meshlet.VertexOffset + threadID.x;
        
        Vertex v = Vertices[vertexIndex];
        outVertices[threadID.x].Position = CalculatePositionCM(v.Position, transform);
        outVertices[threadID.x].Normal = mul(float4(v.Normal, 0.f), transform).xyz;
        outVertices[threadID.x].TexCoord = float2(vertexIndex, groupID.x);
    }

    // Process triangles
    if (threadID.x < meshlet.TriangleCount)
    {
        uint indexOffset = meshlet.IndexOffset + threadID.x * 3;
        outIndices[threadID.x] = uint3(
            Indices[indexOffset], 
            Indices[indexOffset + 1],
            Indices[indexOffset + 2]
        );
    }
}