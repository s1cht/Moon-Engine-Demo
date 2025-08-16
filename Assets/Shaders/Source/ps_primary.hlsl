cbuffer LightBuffer : register(b0, space1)
{
    float4 diffuseColor;
    float3 lightDirection;
    float padding;
}

Texture2D<float4> texture[] : register(t0, space2);
sampler textureSampler[] : register(s0, space2);

struct PS_INPUT
{
    float4 position: SV_POSITION;
	float2 texCoord : TCOORD0;
	float3 normal : NORMALPOS0;
};

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    float3 lightDir;
    float lightIntensity;
    float4 color;
    
    float3 N = normalize(input.normal);
    float3 L = normalize(-lightDirection);
    lightIntensity = saturate(dot(N, L));

    color = saturate(diffuseColor * lightIntensity);

    return color;
}