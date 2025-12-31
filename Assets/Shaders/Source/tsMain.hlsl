#include "Structures.hlsl"

// Objects scales, rotations and positions
StructuredBuffer<MeshMatrix> MeshMatrices : register(t0, space0);
StructuredBuffer<Meshlet> Meshlets : register(t1, space0);

struct Payload
{
    uint MeshletIndex;
    float4x4 Transform;
};

// 16 bytes
struct Constants
{
    uint MaxMeshlets;
    uint3 Padding;
};

[[vk::push_constant]]
Constants constants;
    
[numthreads(32, 1, 1)]
void TSMain(uint3 threadID : SV_GroupThreadID, out Payload payload, out uint meshletCount)
{
    uint meshletIndex = threadID.x;
    meshletCount = 0;

    if (meshletIndex < constants.MaxMeshlets)
    {
        Meshlet currentMeshlet = Meshlets[meshletIndex];
        payload.MeshletIndex = meshletIndex;
        payload.Transform = MeshMatrices[currentMeshlet.MeshIndex].Matrix;
        meshletCount = 1;
        DispatchMesh(1, 1, 1, payload);
    }
}