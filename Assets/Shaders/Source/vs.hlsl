float4 VSMain(uint vertexID : SV_VertexID) : SV_POSITION
{
    const float4 Poses[3] =
    {
        float4(0.f, 0.5f, 1.f, 1.f),
        float4(0.5f, -0.5f, 1.f, 1.f),
        float4(-0.5f, -0.5f, 1.f, 1.f)
    };
    
    return Poses[vertexID];
}