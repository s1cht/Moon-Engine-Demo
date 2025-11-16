struct PSInput
{
    float4 position : SV_POSITION;
    float2 texCoord : TCOORD0;
    float3 normal : NORMALPOS0;
};

float4 PSMain(PSInput input) : SV_TARGET
{
    float4 color;
    color.x = clamp(input.position.x, 0.f, 1.f);
    color.y = clamp(input.position.y, 0.f, 1.f);
    color.z = clamp(input.position.z, 0.f, 1.f);
    color.w = 1.f;
    
    return color;
}