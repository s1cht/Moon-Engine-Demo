#include "Structures.hlsl"

//Texture2D<float4> texture[] : register(t0, space3);
//sampler textureSampler[] : register(s1, space3);

//cbuffer LightBuffer : register(b0, space4)
//{
//    LightData light;
//}

struct PS_INPUT
{
    float4 position: SV_Position;
	float3 normal : NORMALPOS0;
	float2 texCoord : TCOORD0;
    uint MeshID : COLOR0;
};

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    float4 color;
    //float3 lightDir;
    //float lightIntensity;
    //float4 texColor = texture[0].Sample(textureSampler[0], input.texCoord);
    //
    //float3 N = normalize(input.normal);
    //float3 L = normalize(-light.lightDirection);
    //lightIntensity = saturate(dot(N, L));
    //
    //color = saturate(light.diffuseColor * lightIntensity);
    //
    //if (texColor.x == 0.f && texColor.y == 0.f && texColor.z == 0.f)
    //    texColor = float4(1.f, 1.f, 1.f, 1.f);
    //
    //color = color * texColor;

    color.x = clamp(abs(input.position.x), 0.f, 1.f);
    color.y = clamp(abs(input.position.y), 0.f, 1.f);
    color.z = clamp(abs(input.position.z), 0.f, 1.f);
    color.w = 1.f;

    return color;
}