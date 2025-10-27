// Buffer structures

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
};

struct MeshMatrix
{
    float4x4 Matrix;
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

struct Frustum
{
    float4 planes[6];
};

struct CameraData
{
    row_major float4x4 ViewMatrix;
    row_major float4x4 ProjectiomMatrix;
};

struct LightData
{
    float4 diffuseColor;
    float3 lightDirection;
    float padding;
};

struct MeshBoxData
{
    float4 meshCenterPosition;
    float4 extents;
};

struct IndirectDrawDataAndMeshData
{
    DrawIndirectIndexedCommand command;

    uint meshId;
};

struct Line
{
    float3 v0;
    float4 c0;
    float3 v1;
    float4 c1;
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