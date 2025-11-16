#pragma once
#include <Core.hpp>
#include <Core/Containers/Map.hpp>
#include <Core/Memory/Allocators/BuddyAllocator.hpp>

#include "Renderer/Base/Buffer.hpp"
#include "Renderer/Assets/Mesh.hpp"

constexpr SIZE_T ME_MESH_MNG_VERT_BUFFER_SIZE           = ME_MB(BIT(8)); // 256 MB
constexpr SIZE_T ME_MESH_MNG_IND_BUFFER_SIZE            = ME_MB(BIT(7)); // 128 MB
constexpr SIZE_T ME_MESH_MNG_MESHLET_BUFFER_SIZE        = ME_MB(BIT(7)); // 128 MB
constexpr SIZE_T ME_MESH_MNG_MIN_SIZE                   = ME_KB(BIT(8)); // 256 KB
constexpr uint32 ME_MESH_MNG_MAX_MESH_COUNT             = 0xffff;
constexpr uint32 ME_MESH_MNG_MAX_MESH_BOX_COUNT         = 0xffff;
constexpr uint32 ME_MESH_MNG_MESHLET_MAX_VERTEX_COUNT   = 64;
constexpr uint32 ME_MESH_MNG_MESHLET_MAX_TRI_COUNT      = 126;

namespace ME::Assets
{
    class Mesh;
}

namespace ME::Render::Manager
{
    struct MeshMemoryPoolInfo
    {
        SIZE_T VertexMemoryPoolSize;
        SIZE_T IndexMemoryPoolSize;
        SIZE_T MeshletMemoryPoolSize;
        Render::ResourceBinding BindingInfo;
        bool UsingMeshlets = false;
    };

    class MEAPI MeshManager
    {
    public:
        MeshManager();
        MeshManager(const MeshManager&) = delete;
        MeshManager(MeshManager&&) = delete;
        ~MeshManager();

    public:
        MeshManager& operator=(const MeshManager&) = delete;
        MeshManager& operator=(MeshManager&&) = delete;

    public:
        static MeshManager& Get()
        {
            static MeshManager instance;
            return instance;
        }

    public:
        void Init(const MeshMemoryPoolInfo& info) noexcept;
        void Shutdown() noexcept;

        void Clear();

        void ResizeVertexMemPool(SIZE_T size);
        void ResizeIndexMemPool(SIZE_T size);
        void ResizeMeshletMemPool(SIZE_T size);

        ME::Core::Memory::Reference<ME::Render::VertexBuffer> GetVertexBuffer() const { return m_VertexBuffer; }
        ME::Core::Memory::Reference<ME::Render::IndexBuffer> GetIndexBuffer() const { return m_IndexBuffer; }
        ME::Core::Memory::Reference<ME::Render::StorageBuffer> GetMeshletBuffer() const { return m_MeshletBuffer; }
        ME::Core::Memory::Reference<ME::Render::StorageBuffer> GetDrawBuffer() const { return m_DrawBuffer; }
        ME::Core::Memory::Reference<ME::Render::StorageBuffer> GetMeshBoxBuffer() const { return m_MeshBoxBuffer; }

        ME::Core::Array<ME::Assets::DrawMeshData>& GetDrawBufferData() { return m_DrawData; }
        ME::Core::Array<ME::Assets::BoundingBox> GetMeshBoxBufferData() { return m_MeshBoxes; }
        SIZE_T GetMeshCount() const { return m_Meshes.Size(); }

        Render::ResourceLayout GetSetLayout() const { return m_SetLayout; }

    public:
        ME::Core::Memory::Reference<ME::Assets::Mesh> CreateMesh();

        bool LoadMesh(uint64 meshId);
        void UnloadMesh(uint64 meshId);

    private:
        ME::Core::Memory::OAllocation VertexMalloc(SIZE_T size);
        void VertexFree(ME::Core::Memory::OAllocation& alloc);
        ME::Core::Memory::OAllocation IndexMalloc(SIZE_T size);
        void IndexFree(ME::Core::Memory::OAllocation& alloc);
        ME::Core::Memory::OAllocation MeshletMalloc(SIZE_T size);
        void MeshletFree(ME::Core::Memory::OAllocation& alloc);

    private:
        ME::Core::Array<ME::Core::Memory::Reference<ME::Assets::Mesh>> m_Meshes;
        ME::Core::Array<ME::Assets::BoundingBox> m_MeshBoxes;
        ME::Core::Array<ME::Assets::DrawMeshData> m_DrawData;

    private:
        ME::Core::Memory::Reference<ME::Render::VertexBuffer> m_VertexBuffer;
        ME::Core::Memory::Reference<ME::Render::IndexBuffer> m_IndexBuffer;
        ME::Core::Memory::Reference<ME::Render::StorageBuffer> m_MeshletBuffer;
        ME::Core::Memory::Reference<ME::Render::StorageBuffer> m_DrawBuffer;
        ME::Core::Memory::Reference<ME::Render::StorageBuffer> m_MeshBoxBuffer;
        ME::Core::Memory::Scope<ME::Core::Memory::BuddyAllocator> m_VertexAllocator;
        ME::Core::Memory::Scope<ME::Core::Memory::BuddyAllocator> m_IndexAllocator;
        ME::Core::Memory::Scope<ME::Core::Memory::BuddyAllocator> m_MeshletAllocator;

    private:
        Render::ResourceLayout m_SetLayout;
        bool m_UsingMeshlets;

        SIZE_T m_MaxMeshCount;
    };
}
