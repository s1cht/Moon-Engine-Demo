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
};

PS_INPUT VSMain(VS_INPUT input)
{	
    PS_INPUT output;
	float4x4 tempMat;

	tempMat = mul(ViewProjMatrix, WorldMatrix);
	output.position = mul(tempMat, float4(input.position, 1.f));

	return output;
}