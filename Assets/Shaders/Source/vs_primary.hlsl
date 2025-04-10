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
	float4 color : COLOR0;
};

struct PS_INPUT
{
    float4 position: SV_POSITION;
	float4 color : COLOR0;
};

PS_INPUT VSMain(VS_INPUT input)
{	
    PS_INPUT output;
	float4x4 tempMat;

	tempMat = mul(ViewProjMatrix, WorldMatrix);
	output.position = mul(tempMat, float4(input.position, 1.f));
	output.color = input.color;

	return output;
}