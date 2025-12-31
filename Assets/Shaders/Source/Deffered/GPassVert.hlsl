#include "GPassCommon.hlsl"
#include "../Common/Math.hlsl"

// Set 0
StructuredBuffer<Vertex> Vertices : register(t0, space0);
StructuredBuffer<uint> Indices : register(t1, space0);

// Set 1
ConstantBuffer<CameraData> Camera : register(b1, space1);

// Set 2
StructuredBuffer<MeshMatrix> MeshTransforms : register(t0, space2);
StructuredBuffer<uint> MeshIDs : register(t2, space2);

struct VertexShaderInput
{
	float3 Position : POSITION0;
	float3 Normal : NORMALPOS0;
	float4 Tangent : POSITION1;
	float2 TextureCoordinates : TEXCOORD0;
};

VertexShaderResult VSMain(
    VertexShaderInput input,
    uint instanceID : SV_InstanceID
)
{
	VertexShaderResult result;
	result.TexCoords = input.TextureCoordinates;
	result.Normal = normalize(mul(input.Normal, (float3x3) MeshTransforms[instanceID].Matrix)); // v * M
	result.Position = PositionToCameraCM(input.Position, Camera.ProjectionMatrix, Camera.ViewMatrix, MeshTransforms[instanceID].Matrix); // v * W * V * P
	result.Tangent = input.Tangent;
	result.GlobalPosition = mul(float4(input.Position, 1.f), MeshTransforms[instanceID].Matrix); // v * M
	result.MeshID = MeshIDs[instanceID];

	return result;
}