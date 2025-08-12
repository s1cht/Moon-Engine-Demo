cbuffer LightBuffer : register(b0, space1)
{
    float4 diffuseColor;
    float3 lightDirection;
    float padding;
}

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

    lightDir = -lightDirection;
    
    //float3 N = normalize(input.normal);
    //float3 L = normalize(-lightDirection);
    //lightIntensity = saturate(dot(N, L));

    color = saturate(diffuseColor * 1.f);

    return color;
}