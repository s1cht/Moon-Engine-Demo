// Buffer structures
#include "Globals.hlsl"

#define COMBINED_SAMPLER
#ifdef __spirv__
    #define COMBINED_SAMPLER [[vk::combinedImageSampler]]
#endif

#define MATRIX_ORDER column_major

struct BoundingBox
{
    float3 Center;
	uint Padding1;

    float3 Extents;
	uint Padding2;
};

struct BoundingSphere
{
    float3 Center;
    float Radius;
};

// Mesh data
struct Vertex
{
    float3 Position;
	uint Padding1;
    
    float3 Normal;
	uint Padding2;
	
    float4 Tangent;
    
    float2 TextureCoordinates;
    uint Padding3[2];
};

struct Meshlet
{
    uint VertexOffset;
    uint IndexOffset;
    uint VertexCount;
    uint TriangleCount;
    uint MeshIndex;
    uint Padding[3];
    BoundingSphere Sphere;
};

struct MeshMatrix
{
    MATRIX_ORDER float4x4 Matrix;
};

struct DrawIndirectCommand
{
    uint vertexCount;
    uint instanceCount;
    uint firstVertex;
    uint firstInstance;
};

struct DrawIndirectIndexedCommand
{
    uint indexCount;
    uint instanceCount;
    uint firstIndex;
    uint vertexOffset;
    uint firstInstance;
};

struct DrawMeshData
{
    uint VertexOffset;
    uint MeshletOffset;
    uint IndexOffset;
    uint MeshletCount;

    uint VertexCount;
    uint IndexCount;
    uint MeshID;
	uint Padding;
};

struct Frustum
{
    float4 Planes[6];
};

struct CameraData
{
    MATRIX_ORDER float4x4 ViewMatrix;
    MATRIX_ORDER float4x4 ProjectionMatrix;
    MATRIX_ORDER float4x4 InvertedViewMatrix;
	MATRIX_ORDER float4x4 InvertedProjectionMatrix;
};

struct DirectionalLight
{
	uint ColorAndEnabled;
	float Brightness;
	float4 Direction;
	uint Padding[3];
};

struct PointLight
{
	uint ColorAndEnabled;
	float Brightness;
	float Diameter;
	uint Padding1;
	float3 Position;
	uint Padding2;
};

struct SpotLight
{
	uint ColorAndEnabled;
	float Brightness;
	float Angle;
	float Length;

	float3 Position;
	uint Padding1;
	float3 Direction;
	uint Padding2;
};

template<typename T>
struct RWType
{
	T Value;
};