#include "../Common/Math.hlsl"
#include "DefferedCommon.hlsl"

StructuredBuffer<DirectionalLight> DirectionalLightData : register(t0, space0);
StructuredBuffer<PointLight> PointLightData : register(t1, space0);
StructuredBuffer<SpotLight> SpotLightData : register(t2, space0);

COMBINED_SAMPLER Texture2D g_BaseColor : register(t0, space4);
COMBINED_SAMPLER sampler g_BaseColorSampler : register(s0, space4);
COMBINED_SAMPLER Texture2D g_Normal : register(t1, space4);
COMBINED_SAMPLER sampler g_NormalSampler : register(s1, space4);
COMBINED_SAMPLER Texture2D g_WorldPosition : register(t2, space4);
COMBINED_SAMPLER sampler g_WorldPositionSampler : register(s2, space4);
COMBINED_SAMPLER Texture2D g_Depth : register(t5, space4);
COMBINED_SAMPLER sampler g_DepthSampler : register(s5, space4);


struct Constants
{
	uint MaxCount;
	uint DirectionalLightCount;
	uint PointLightCount;
	uint SpotLightCount;
	float3 CameraPosition;
	float Padding;
	float3 WarmColor;
	float Padding1;
};

[[vk::push_constant]]
Constants Constants;

float3 UnlitColor()
{
	return float4(0.1f, 0.1f, 0.1f, 1.f);
}

float3 LitColor(float3 l, float3 n, float3 v)
{
	float3 r_l = reflect(-l, n);
	float s = clamp(100.0 * dot(r_l, v) - 97.0, 0.0, 1.0);
	float3 highlightColor = float3(1, 1, 1);
	return lerp(Constants.WarmColor, highlightColor, s);
}

float DistanceFactor(float distance, float maxDistance)
{
	float ratio = distance / maxDistance;
	float ratio4 = ratio * ratio * ratio * ratio;
	float falloff = saturate(1.f - ratio4);
	return falloff * falloff;
}

inline float UmbraFactor(float angle)
{
	return cos(angle);
}

inline float PenumbraFactor(float angle)
{
	return cos(angle * 0.8);
}

float SpotFalloff(float angle, float3 lightDirection, float3 directionToSurface)
{
	float umbraFactor = UmbraFactor(angle);
	float falloffRoot = saturate((dot(lightDirection, -directionToSurface) - umbraFactor) / (PenumbraFactor(angle) - umbraFactor));
	return falloffRoot * falloffRoot;
}

float4 PSMain(DefferedVertexOutput input) : SV_TARGET
{
	float3 baseColor = g_BaseColor.Sample(g_BaseColorSampler, input.ScreenPosition).xyz;
	float3 normal = g_Normal.Sample(g_NormalSampler, input.ScreenPosition).xyz;
	float3 worldPos = g_WorldPosition.Sample(g_WorldPositionSampler, input.ScreenPosition).xyz;

	float3 viewVectorUnnormalized = Constants.CameraPosition - worldPos;
	float3 viewVector = normalize(viewVectorUnnormalized);
    
	float3 calculatedLightColor = UnlitColor();

	for (uint i = 0; i < Constants.DirectionalLightCount; ++i)
	{
		DirectionalLight light = DirectionalLightData[i];
		float3 color;
		bool enabled;
		ExtractColorAndEnabledFromColor3(light.ColorAndEnabled, color, enabled);
		if (!enabled)
			continue;
        
		float3 L = normalize(-light.Direction.xyz);
		float ndl = saturate(dot(normal, L));
		calculatedLightColor += LitColor(L, normal, viewVector) * ndl * color * light.Brightness;
	}

	for (uint i = 0; i < Constants.PointLightCount; ++i)
	{
		PointLight light = PointLightData[i];
		float3 color;
		bool enabled;
		ExtractColorAndEnabledFromColor3(light.ColorAndEnabled, color, enabled);
		if (!enabled)
			continue;

		float3 L_unnorm = light.Position - worldPos;
		float dist = length(L_unnorm);
		float3 L = L_unnorm / dist; 
        
		float ndl = saturate(dot(normal, L));
		float falloff = DistanceFactor(dist, light.Diameter / 2.0f);
        
		calculatedLightColor += LitColor(L, normal, viewVector) * ndl * color * falloff * light.Brightness;
	}

	for (uint i = 0; i < Constants.SpotLightCount; ++i)
	{
		SpotLight light = SpotLightData[i];
		float3 color;
		bool enabled;
		ExtractColorAndEnabledFromColor3(light.ColorAndEnabled, color, enabled);
		if (!enabled || light.Angle <= 0)
			continue;

		float3 L_unnorm = light.Position - worldPos;
		float dist = length(L_unnorm);
		float3 L = L_unnorm / dist;

		float ndl = saturate(dot(normal, L));
		float distFalloff = DistanceFactor(dist, light.Length);
        
		float sFalloff = SpotFalloff(light.Angle, normalize(light.Direction), L);

		calculatedLightColor += LitColor(L, normal, viewVector) * ndl * color * distFalloff * sFalloff * light.Brightness;
	}

	return float4(calculatedLightColor * baseColor, 1.f);
}