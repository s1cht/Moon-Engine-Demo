cbuffer CameraBuffer : register(b0, space0)
{
    row_major float4x4 ViewMatrix;
    row_major float4x4 ProjectiomMatrix;
}

cbuffer SceneBuffer : register(b1, space0)
{
    row_major float4x4 WorldMatrix;
}

struct VS_INPUT
{
	float3 position : POSITION0;
	float2 texCoord : TCOORD0;
	float3 normal : NORMALPOS0;
};

struct PS_INPUT
{
    float4 position: SV_POSITION;
	float2 texCoord : TCOORD0;
	float3 normal : NORMALPOS0;
};

// Column-Major
inline float4 CalculatePositionCM(float3 position)
{
    return mul(mul(mul(ProjectiomMatrix, ViewMatrix), WorldMatrix), float4(position, 1.f));
}

// Row-Major
float4 CalculatePositionRM(float3 position)
{
    return mul(mul(mul(float4(position, 1.f), WorldMatrix), ViewMatrix), ProjectiomMatrix);
}


PS_INPUT VSMain(VS_INPUT input)
{	
    PS_INPUT output;

	// Vertex position
    output.position = CalculatePositionCM(input.position);
	
	// UV Coords
	output.texCoord = input.texCoord;

	// Normal calculation
	output.normal = mul(input.normal, (float3x3)WorldMatrix);
	output.normal = normalize(output.normal);

	return output;
}