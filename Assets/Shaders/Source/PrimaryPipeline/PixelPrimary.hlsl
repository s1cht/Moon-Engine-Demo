#include "PrimaryCommon.hlsl"

// Set 2
StructuredBuffer<MeshShadingInfo> MeshShadingInfo : register(t1, space2);

float4 PSMain(VertexShaderResult input) : SV_TARGET
{
    float3 N = normalize(input.Normal);
    float3 L = normalize(-float3(0, -1.f, 0));
    float lightIntensity = saturate(dot(N, L));
    float4 color = saturate(float4(1.f, 0.f, 0.f, 1.f) * lightIntensity);

    return color;
}