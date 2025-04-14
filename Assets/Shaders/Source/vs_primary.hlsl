cbuffer CameraBuffer : register(b0)
{
	float4x4 ViewProjMatrix;
}

cbuffer SceneBuffer : register(b1)
{
	float4x4 WorldMatrix;
}

struct VS_INPUT
{
	float3 position : POSITION0;
	float2 texCoord : TCOORD0;
	float3 normal : NORMALPOS0;
};

struct PS_INPUT
{
    float4 position: SV_POSITION;
	float2 texCoord : TCOORD0;
	float3 normal : NORMALPOS0;
};

PS_INPUT VSMain(VS_INPUT input)
{	
    PS_INPUT output;

	// Vertex position
	float4x4 tempMat;
	tempMat = mul(ViewProjMatrix, WorldMatrix);
	output.position = mul(tempMat, float4(input.position, 1.f));

	// UV Coords
	output.texCoord = input.texCoord;

	// Normal calculation
	output.normal = mul(input.normal, (float3x3)WorldMatrix);
	output.normal = normalize(output.normal);

	return output;
}