#include "Structures.hlsl"
#include "Globals.hlsl"

// Buffers
// Set 0
StructuredBuffer<Vertex> Vertices : register(t0, space0);
StructuredBuffer<uint> Indices : register(t1, space0);
StructuredBuffer<Meshlet> Meshlets : register(t2, space0);
//StructuredBuffer<MeshBoxData> MeshBoxes : register(t3, space0); -- unused
// Set 1
cbuffer CamData : register(b0, space1)
{
    CameraData CameraData;
}

struct Payload
{
    uint MeshletIndex;  
    float4x4 Transform;
};

struct MeshOutputVertex
{
    float4 Position : SV_Position;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD0;
};

// Column-Major
inline float4 CalculatePositionCM(float3 position, float4x4 objectTransform)
{
    return mul(mul(mul(CameraData.ProjectiomMatrix, CameraData.ViewMatrix), objectTransform), float4(position, 1.f));
}

// Row-Major
float4 CalculatePositionRM(float3 position, float4x4 objectTransform)
{
    return mul(mul(mul(float4(position, 1.f), objectTransform), CameraData.ViewMatrix), CameraData.ProjectiomMatrix);
}

[NumThreads(128, 1, 1)]
[OutputTopology("triangle")]
void MSMain(
    uint3 threadID : SV_GroupThreadID,
    in Payload payload,
    out vertices MeshOutputVertex outVertices[MS_MAX_VERTS],
    out indices uint3 outIndices[MS_MAX_TRIANGLES]
)
{
    uint meshletIndex = payload.MeshletIndex;
    Meshlet meshlet = Meshlets[meshletIndex];

    // Set output counts
    SetMeshOutputCounts(meshlet.VertexCount, meshlet.TriangleCount);

    // Process vertices
    if (threadID.x < meshlet.VertexCount)
    {
        uint vertexIndex = meshlet.VertexOffset + threadID.x;
        Vertex v = Vertices[vertexIndex];

        outVertices[threadID.x].Position = CalculatePositionCM(v.Position, payload.Transform);
        outVertices[threadID.x].Normal = mul(float4(v.Normal, 0.0), payload.Transform).xyz;
        outVertices[threadID.x].TexCoord = v.TextureCoordinates;
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