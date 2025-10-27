#include "Structures.hlsl"
#include "Globals.hlsl"

struct VS_INPUT
{
    float3 position : POSITION0;
    float2 texCoord : TCOORD0;
    float3 normal : NORMALPOS0;
};

StructuredBuffer<VS_INPUT> vertexAlias  : register(t0, space0);
StructuredBuffer<uint>     indexAlias   : register(t1, space0);
StructuredBuffer<IndirectDrawDataAndMeshData> drawAlias : register(t2, space0);

cbuffer CameraBuffer : register(b0, space1)
{
    CameraData camera;
}

cbuffer SceneBuffer : register(b1, space1)
{
    SceneData scene;
}

struct PS_INPUT
{
    float4 position: SV_POSITION;
	float2 texCoord : TCOORD0;
	float3 normal : NORMALPOS0;
    nointerpolation uint meshId : TEXCOORD1;
};

// Column-Major
inline float4 CalculatePositionCM(float3 position)
{
    return mul(mul(mul(camera.ProjectiomMatrix, camera.ViewMatrix), scene.WorldMatrix), float4(position, 1.f));
}

// Row-Major
float4 CalculatePositionRM(float3 position)
{
    return mul(mul(mul(float4(position, 1.f), scene.WorldMatrix), camera.ViewMatrix), camera.ProjectiomMatrix);
}

[outputtopology("triangle")]
[numthreads(32, 1, 1)]
void MSMain(uint3 threadId : SV_GroupThreadID,
    uint drawIndex : SV_DrawID,
    out vertices PS_INPUT verts[MS_MAX_VERTS],
    out indices uint3 triangles[MS_MAX_TRIANGLES])
{
    uint index = indexAlias[vertexIndex];
    VS_INPUT input = vertexAlias[index];

    PS_INPUT output;

	// Vertex position
    output.position = CalculatePositionCM(input.position);
	
	// UV Coords
	output.texCoord = input.texCoord;

	// Normal calculation
    output.normal = mul(input.normal, (float3x3)scene.WorldMatrix);
	output.normal = normalize(output.normal);

    output.meshId = drawAlias[drawIndex].meshId;
}