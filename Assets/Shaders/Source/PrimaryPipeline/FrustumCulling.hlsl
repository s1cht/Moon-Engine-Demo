#include "../Common/Math.hlsl"

StructuredBuffer<BoundingBox> MeshBoxes : register(t4, space0);

ConstantBuffer<Frustum> Frustum : register(b0, space1);
ConstantBuffer<CameraData> Camera : register(b1, space1);

// Set 2
StructuredBuffer<MeshMatrix> MeshTransforms : register(t0, space2);

StructuredBuffer<DrawIndirectIndexedCommand> InputIndirectDraws : register(t0, space3);
RWStructuredBuffer<DrawIndirectIndexedCommand> OutputIndirectDraws : register(u1, space3);
RWStructuredBuffer< RWType<uint> > OutputIndirectCountBuffer : register(u2, space3);

struct Constants
{
    uint count;
};

[[vk::push_constant]]
Constants constants;

void CullMesh(uint meshId, uint instanceId)
{
    BoundingBox meshBox = MeshBoxes[meshId];
	meshBox.Center = PositionToCameraCM(meshBox.Center, Camera.ProjectionMatrix, Camera.ViewMatrix, MeshTransforms[instanceId].Matrix);
    bool isVisible = true;

    for (int i = 0; i < 6 && isVisible; i++)
    {
        float3 planeNormal = Frustum.Planes[i].xyz;
        float distanceFromPlane = dot(meshBox.Center, planeNormal);

        float absDiff = dot(abs(planeNormal), meshBox.Extents.xyz);
        if (distanceFromPlane + absDiff + Frustum.Planes[i].w < 0.f)
            isVisible = false;
    }

	if (isVisible)
    {
        uint index;
        InterlockedAdd(OutputIndirectCountBuffer[0].Value, 1, index);
        OutputIndirectDraws[index] = InputIndirectDraws[meshId];
    }
}

[numthreads(64, 4, 1)]
void CSMain(uint3 id : SV_DispatchThreadID)
{
	uint meshId = id.x;
	DrawIndirectIndexedCommand drawCommand = InputIndirectDraws[meshId];
	uint instanceId = id.y + drawCommand.firstInstance;

    if (instanceId >= drawCommand.instanceCount)
		return;

    uint oldValue;
    if (meshId == 0)
		InterlockedExchange(OutputIndirectCountBuffer[0].Value, 0, oldValue);

    AllMemoryBarrier();

	if (meshId.x < constants.count)
        CullMesh(meshId, instanceId);
}