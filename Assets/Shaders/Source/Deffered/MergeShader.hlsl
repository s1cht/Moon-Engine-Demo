#include "../Common/Structures.hlsl"
#include "GPassCommon.hlsl"

COMBINED_SAMPLER Texture2D g_BaseColor : register(t0, space4);
COMBINED_SAMPLER sampler g_BaseColorSampler : register(s0, space4);

struct MergeVO
{
	float4 Position : SV_POSITION;
	float2 ScreenPosition : TEXCOORD0;
};

MergeVO VSMain(uint vertexId : SV_VertexID)
{
	const float2 positions[4] = {
	    float2(-1.f, -1.f), float2(-1.f, 1.f),
	    float2(1.f, -1.f), float2(1.f, 1.f)
	};
	const float2 texPositions[4] =
	{
		float2(0.f, 0.f), float2(0.f, 1.f),
	    float2(1.f, 0.f), float2(1.f, 1.f)
	};

	MergeVO result;
	result.Position = float4(positions[vertexId], 0.f, 1.f);
	result.ScreenPosition = texPositions[vertexId];

	return result;
}

float4 PSMain(MergeVO input) : SV_TARGET
{
	float4 baseColor = g_BaseColor.Sample(g_BaseColorSampler, input.ScreenPosition);
	return baseColor;
}