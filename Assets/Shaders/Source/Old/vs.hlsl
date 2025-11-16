struct VSInput
{
    float3 position : POSITION0;
    float2 texCoord : TCOORD0;
    float3 normal : NORMALPOS0;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 texCoord : TCOORD0;
    float3 normal : NORMALPOS0;
};
    
PSInput VSMain(VSInput input)
{
    PSInput output;
    output.position = float4(input.position, 1.f);
    output.texCoord = input.texCoord;


    return output;
}