#include "Globals.hlsl"
#include "Structures.hlsl"


// Column-Major
inline float4 PositionToCameraCM(float3 position, float4x4 projectionMatrix, float4x4 viewMatrix, float4x4 transformMatrix)
{
	float4x4 step1 = mul(projectionMatrix, viewMatrix);
	float4x4 step2 = mul(step1, transformMatrix);
	float4 result = mul(step2, float4(position, 1.f));
	return result;
}
inline float4 PositionToCameraCM(float4 position, float4x4 projectionMatrix, float4x4 viewMatrix, float4x4 transformMatrix)
{
	float4x4 step1 = mul(projectionMatrix, viewMatrix);
	float4x4 step2 = mul(step1, transformMatrix);
	float4 result = mul(step2, position);
	return result;
}

// Row-Major
float4 PositionToCameraRM(float3 position, float4x4 projectionMatrix, float4x4 viewMatrix, float4x4 transformMatrix)
{
	float4 step1 = mul(float4(position, 1.f), transformMatrix);
	float4 step2 = mul(step1, viewMatrix);
	float4 result = mul(step2, projectionMatrix);
	return result;
}
float4 PositionToCameraRM(float4 position, float4x4 projectionMatrix, float4x4 viewMatrix, float4x4 transformMatrix)
{
	float4 step1 = mul(position, transformMatrix);
	float4 step2 = mul(step1, viewMatrix);
	float4 result = mul(step2, projectionMatrix);
	return result;
}