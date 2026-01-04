#include "../Common/Structures.hlsl"
#include "DefferedCommon.hlsl"
#include "GPassCommon.hlsl"

COMBINED_SAMPLER Texture2D RenderResult : register(t0, space5);
COMBINED_SAMPLER sampler RenderResultSampler : register(s0, space5);

float4 PSMain(DefferedVertexOutput input) : SV_TARGET
{
	float4 baseColor = RenderResult.Sample(RenderResultSampler, input.ScreenPosition);
	return baseColor;
}