#include "Globals.hlsl"
#include "Structures.hlsl"


// Column-Major
inline float4 PositionToCameraCM(float3 position, float4x4 projectionMatrix, float4x4 viewMatrix, float4x4 transformMatrix)
{
	return mul(mul(mul(projectionMatrix, viewMatrix), transformMatrix), float4(position, 1.f));
}
inline float4 PositionToCameraCM(float4 position, float4x4 projectionMatrix, float4x4 viewMatrix, float4x4 transformMatrix)
{
	return mul(mul(mul(projectionMatrix, viewMatrix), transformMatrix), position);
}

// Row-Major
float4 PositionToCameraRM(float3 position, float4x4 projectionMatrix, float4x4 viewMatrix, float4x4 transformMatrix)
{
	return mul(mul(mul(float4(position, 1.f), transformMatrix), viewMatrix), projectionMatrix);
}
float4 PositionToCameraRM(float4 position, float4x4 projectionMatrix, float4x4 viewMatrix, float4x4 transformMatrix)
{
	return mul(mul(mul(position, transformMatrix), viewMatrix), projectionMatrix);
}