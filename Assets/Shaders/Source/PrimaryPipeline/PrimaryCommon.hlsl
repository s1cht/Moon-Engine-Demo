struct VertexShaderResult
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMALPOS0;
    float2 TexCoords : TEXCOORD0;
    nointerpolation uint MeshID : TEXCOORD1;
};

struct MeshShadingInfo
{
    float Opacity;
    bool ShadowsVisible;
};
