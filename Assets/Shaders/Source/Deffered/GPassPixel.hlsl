#include "GPassCommon.hlsl"

// Set 2
StructuredBuffer<MeshShadingInfo> MeshShadingInfo : register(t1, space2);

struct PixelShaderResult
{
	float4 BaseColor : SV_Target0;
	float4 Normal : SV_Target1;
};

PixelShaderResult PSMain(VertexShaderResult input) : SV_TARGET
{
	PixelShaderResult result;
	result.BaseColor = float4(1.f, 1.f, 1.f, 1.f);
	result.Normal = float4(input.Normal, 1.f);

    return result;
}