struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    return float4(1.f, 0.f, 0.f, 1.f);
}