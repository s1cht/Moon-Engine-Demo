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
    float3 Extents;
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
    float3 Normal;
	float4 Tangent;
    float2 TextureCoordinates;
};

struct Meshlet
{
    uint VertexOffset;
    uint IndexOffset;
    uint VertexCount;
    uint TriangleCount;
    uint MeshIndex;
    BoundingSphere Sphere;
    uint Padding[3];
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

struct LightData
{
    float4 diffuseColor;
    float3 lightDirection;
    float padding;
};

struct IndirectDrawDataAndMeshData
{
    DrawIndirectIndexedCommand Command;
    uint MeshId;
};

struct Line
{
    float3 v0;
    float4 c0;
    float3 v1;
    float4 c1;
};

struct MeshletPayload
{
    uint MeshletIndex;
    uint3 Padding;
};

struct DebugMessages
{
    uint size;
    uint row;
    uint pad1;
    uint pad2;
    DrawIndirectCommand command;
    //Line lines[];
};

template<typename T>
struct RWType
{
	T Value;
};