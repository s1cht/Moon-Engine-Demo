// Buffer structures
#include "Globals.hlsl"

#pragma pack_matrix(row_major)

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
    float2 TextureCoordinates;
    float3 Normal;
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
    row_major float4x4 Matrix;
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
    row_major float4x4 ViewMatrix;
    row_major float4x4 ProjectionMatrix;
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