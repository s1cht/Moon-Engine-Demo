#include "MeshManager.hpp"

#include "Renderer/RenderCommand.hpp"

namespace ME::Render::Manager
{
    MeshManager::MeshManager()
        : m_Meshes(), m_VertexBuffer(nullptr), m_IndexBuffer(nullptr), m_VertexAllocator(nullptr), m_IndexAllocator(nullptr)
    {
    }

    MeshManager::~MeshManager()
    {
        if (m_VertexBuffer != nullptr) m_VertexBuffer->Shutdown();
        if (m_IndexBuffer != nullptr) m_IndexBuffer->Shutdown();
        if (m_DrawBuffer != nullptr) m_DrawBuffer->Shutdown();
        if (m_MeshBoxBuffer != nullptr) m_MeshBoxBuffer->Shutdown();
    }

    void MeshManager::Init(const MeshMemoryPoolInfo& info) noexcept
    {
        m_UsingMeshlets = info.UsingMeshlets;

        Core::Memory::BuddyAllocatorSpecification vertexSpecs = {};
        vertexSpecs.MinBlockSize = ME_MESH_MNG_MIN_SIZE;
        vertexSpecs.TotalSize = info.VertexMemoryPoolSize;

        Core::Memory::BuddyAllocatorSpecification indexSpecs = {};
        indexSpecs.MinBlockSize = ME_MESH_MNG_MIN_SIZE;
        indexSpecs.TotalSize = info.IndexMemoryPoolSize;

        Core::Memory::BuddyAllocatorSpecification meshletSpecs = {};
        meshletSpecs.MinBlockSize = ME_MESH_MNG_MIN_SIZE;
        meshletSpecs.TotalSize = info.MeshletMemoryPoolSize;

        m_MaxMeshCount = ME_MESH_MNG_MAX_MESH_COUNT;

        m_VertexAllocator = ME::Core::Memory::MakeScope<Core::Memory::BuddyAllocator>();
        m_IndexAllocator = ME::Core::Memory::MakeScope<Core::Memory::BuddyAllocator>();
        m_MeshletAllocator = ME::Core::Memory::MakeScope<Core::Memory::BuddyAllocator>();

        Core::Memory::BuddyAllocatorErrors result = m_VertexAllocator->Init(vertexSpecs);
        if (result != Core::Memory::BuddyAllocatorErrors::Success)
        {
            ME_ASSERT(false, "Vertex allocator creation failed! Error: {}", static_cast<uint32>(result));
            return;
        }
        result = m_IndexAllocator->Init(indexSpecs);
        if (result != Core::Memory::BuddyAllocatorErrors::Success)
        {
            ME_ASSERT(false, "Index allocator creation failed! Error: {}", static_cast<uint32>(result));
            return;
        }
        result = m_MeshletAllocator->Init(meshletSpecs);
        if (result != Core::Memory::BuddyAllocatorErrors::Success)
        {
            ME_ASSERT(false, "Meshlet allocator creation failed! Error: {}", static_cast<uint32>(result));
            return;
        }

        m_SetLayout = ResourceLayout(5);
        m_SetLayout.Clear();

        // Vertices
        m_SetLayout.EmplaceBack(info.BindingInfo);
        // Indices
        m_SetLayout.EmplaceBack(info.BindingInfo);
        // Meshlets
        m_SetLayout.EmplaceBack(info.BindingInfo);
        // Mesh boxes
        m_SetLayout.EmplaceBack(info.BindingInfo);
        // Draw info
        m_SetLayout.EmplaceBack(info.BindingInfo);

        uint32 setIndex = Render::RenderCommand::GetResourceHandler()->CreateResourceSet(m_SetLayout);

        VertexBufferSpecification vertexMemPoolSpecs = {};
        vertexMemPoolSpecs.Set = 0;
        vertexMemPoolSpecs.Binding = 0;
        vertexMemPoolSpecs.ResourceBinding = info.BindingInfo;
        vertexMemPoolSpecs.SetIndex = setIndex;
        vertexMemPoolSpecs.Size = info.VertexMemoryPoolSize;
        vertexMemPoolSpecs.DebugName = "Vertex memory pool";

        IndexBufferSpecification indexMemPoolSpecs = {};
        indexMemPoolSpecs.Set = 0;
        indexMemPoolSpecs.Binding = 1;
        indexMemPoolSpecs.ResourceBinding = info.BindingInfo;
        indexMemPoolSpecs.SetIndex = setIndex;
        indexMemPoolSpecs.IndexCount = info.IndexMemoryPoolSize / 4;
        indexMemPoolSpecs.DebugName = "Index memory pool";

        StorageBufferSpecification meshletBufferSpecs = {};
        meshletBufferSpecs.Set = 0;
        meshletBufferSpecs.Binding = 2;
        meshletBufferSpecs.ResourceBinding = info.BindingInfo;
        meshletBufferSpecs.SetIndex = setIndex;
        meshletBufferSpecs.Size = ME_MESH_MNG_MESHLET_BUFFER_SIZE;
        meshletBufferSpecs.MemoryType = MemoryType::VRAM;
        meshletBufferSpecs.DebugName = "Meshlet buffer";

        StorageBufferSpecification meshBoxSpecs = {};
        meshBoxSpecs.Set = 0;
        meshBoxSpecs.Binding = 3;
        meshBoxSpecs.ResourceBinding = info.BindingInfo;
        meshBoxSpecs.SetIndex = setIndex;
        meshBoxSpecs.Size = sizeof(ME::Assets::BoundingBox) * ME_MESH_MNG_MAX_MESH_COUNT;
        meshBoxSpecs.MemoryType = MemoryType::VRAM;
        meshBoxSpecs.DebugName = "Mesh box buffer";

        StorageBufferSpecification drawInputSpecs = {};
        drawInputSpecs.Set = 0;
        drawInputSpecs.Binding = 4;
        drawInputSpecs.ResourceBinding = info.BindingInfo;
        drawInputSpecs.SetIndex = setIndex;
        drawInputSpecs.Size = sizeof(ME::Assets::DrawMeshData) * ME_MESH_MNG_MAX_MESH_COUNT;
        drawInputSpecs.MemoryType = MemoryType::VRAM;
        drawInputSpecs.DebugName = "Draw input buffer";

        m_VertexBuffer = ME::Render::VertexBuffer::Create(vertexMemPoolSpecs);
        m_IndexBuffer = ME::Render::IndexBuffer::Create(indexMemPoolSpecs);
        m_MeshletBuffer = ME::Render::StorageBuffer::Create(meshletBufferSpecs);
        m_MeshBoxBuffer = ME::Render::StorageBuffer::Create(meshBoxSpecs);
        m_DrawBuffer = ME::Render::StorageBuffer::Create(drawInputSpecs);

        m_MeshBoxes = ME::Core::Array<ME::Assets::BoundingBox>();
        m_MeshBoxes.Reserve(ME_MESH_MNG_MAX_MESH_COUNT);

        m_DrawData = ME::Core::Array<ME::Assets::DrawMeshData>();
        m_DrawData.Reserve(ME_MESH_MNG_MAX_MESH_COUNT);
        
        m_MeshBoxBuffer = Render::StorageBuffer::Create(meshBoxSpecs);
        
        m_DrawBuffer = Render::StorageBuffer::Create(drawInputSpecs);
    }

    void MeshManager::Shutdown() noexcept
    {
        m_VertexBuffer->Shutdown();
        m_IndexBuffer->Shutdown();
    }

    void MeshManager::Clear() 
    {
        m_VertexBuffer->Clear();
        m_VertexAllocator->Clear();

        m_IndexBuffer->Clear();
        m_IndexAllocator->Clear();

        for (const auto& mesh : m_Meshes)
            mesh->SetLoaded(false);
    }

    void MeshManager::ResizeVertexMemPool(SIZE_T size)
    {
        if (m_VertexBuffer->GetSpecification().Size <= size) return;
        if (!m_VertexAllocator->Resize(size))
            return;
        m_VertexBuffer->Resize(size);
    }

    void MeshManager::ResizeIndexMemPool(SIZE_T size) 
    {
        if (size % sizeof(uint32) != 0)
        {
            ME_ASSERT(false, "Can't divide size by size of uint32!");
            return;
        }
        if (m_IndexBuffer->GetCount() <= size / sizeof(uint32)) return;
        if (!m_IndexAllocator->Resize(size))
            return;
        m_IndexBuffer->Resize(size / sizeof(uint32));
    }

    void MeshManager::ResizeMeshletMemPool(SIZE_T size)
    {
        if (size % sizeof(ME::Assets::Meshlet) != 0)
        {
            ME_ASSERT(false, "Can't divide size by size of Meshlet!");
            return;
        }
        if (m_MeshletBuffer->GetSpecification().Size <= size) return;
        if (!m_MeshletAllocator->Resize(size))
            return;
        m_MeshletBuffer->Resize(size);
    }

    ME::Core::Memory::Reference<ME::Assets::Mesh> MeshManager::CreateMesh()
    {
        ME::Core::Memory::Reference<ME::Assets::Mesh> mesh = ME::Assets::Mesh::Create();
        m_Meshes.EmplaceBack(mesh);
        mesh->SetMeshID(m_Meshes.Size() - 1);
        return mesh;
    }

    bool MeshManager::LoadMesh(uint64 meshId)
    {
        if (m_MeshBoxes.Size() >= m_MaxMeshCount) return false;

        ME::Core::Memory::Reference<ME::Assets::Mesh>& mesh = m_Meshes[meshId];
        Core::Memory::OAllocation vertexAlloc = VertexMalloc(mesh->GetVertices().Size() * sizeof(Assets::Vertex));
        if (vertexAlloc == nullptr)
        {
            VertexFree(vertexAlloc);
            return false;
        }

        Core::Memory::OAllocation indexAlloc = IndexMalloc(mesh->GetIndices().Size() * sizeof(uint32));
        if (indexAlloc == nullptr)
        {
            IndexFree(indexAlloc);
            return false;
        }

        Core::Memory::OAllocation meshletAlloc = nullptr;
        if (m_UsingMeshlets)
        {
            meshletAlloc = MeshletMalloc(mesh->GetMeshlets().Size() * sizeof(ME::Assets::Meshlet));
            if (meshletAlloc == nullptr)
            {
                MeshletFree(meshletAlloc);
                return false;
            }
        }

        mesh->SetVertexAllocation(vertexAlloc);
        m_VertexBuffer->SetData(mesh->GetVertices().Data(), vertexAlloc->Size, vertexAlloc->Offset);

        mesh->SetIndexAllocation(indexAlloc);
        m_IndexBuffer->SetData(mesh->GetIndices().Data(), mesh->GetIndices().Size(), indexAlloc->Offset / sizeof(uint32));

        mesh->SetMeshletAllocation(meshletAlloc);
        if (m_UsingMeshlets)
            m_MeshletBuffer->SetData(mesh->GetMeshlets().Data(), mesh->GetMeshlets().Size() * sizeof(ME::Assets::Meshlet), meshletAlloc->Offset);

        ME::Assets::DrawMeshData drawData = mesh->GetDrawData();
        drawData.VertexCount = static_cast<uint32>(vertexAlloc->Size / sizeof(ME::Assets::Vertex));
        drawData.VertexOffset = static_cast<uint32>(vertexAlloc->Offset / sizeof(ME::Assets::Vertex));
        drawData.IndexCount = static_cast<uint32>(indexAlloc->Size / sizeof(uint32));
        drawData.IndexOffset = static_cast<uint32>(indexAlloc->Offset / sizeof(uint32));

        if (m_UsingMeshlets)
        {
            drawData.MeshletCount = static_cast<uint32>(meshletAlloc->Size / sizeof(ME::Assets::Meshlet));
            drawData.MeshletOffset = static_cast<uint32>(meshletAlloc->Offset / sizeof(ME::Assets::Meshlet));
        }
        else
        {
            drawData.MeshletCount = 0;
            drawData.MeshletOffset = 0;
        }

        mesh->SetDrawData(drawData);

        SIZE_T drawOffset = meshId * sizeof(ME::Assets::DrawMeshData);
        m_DrawData[meshId] = drawData;
        m_DrawBuffer->SetData(&drawData, sizeof(ME::Assets::DrawMeshData), drawOffset);

        ME::Assets::BoundingBox meshBox = mesh->GetMeshBox();
        SIZE_T meshBoxOffset = meshId * sizeof(ME::Assets::BoundingBox);
        m_MeshBoxes[meshId] = meshBox;
        m_MeshBoxBuffer->SetData(&meshBox, sizeof(ME::Assets::BoundingBox), meshBoxOffset);

        return true;
    }

    void MeshManager::UnloadMesh(uint64 meshId)
    {
        ME::Core::Memory::Reference<ME::Assets::Mesh>& mesh = m_Meshes[meshId];
        Core::Memory::OAllocation vertexAlloc = mesh->GetVertexAllocation();
        Core::Memory::OAllocation indexAlloc = mesh->GetIndexAllocation();
        Core::Memory::OAllocation meshletAlloc = mesh->GetMeshletAllocation();

        if (vertexAlloc)
        {
            m_VertexAllocator->Free(vertexAlloc);
            mesh->SetVertexAllocation(nullptr);
        }

        if (indexAlloc)
        {
            m_IndexAllocator->Free(indexAlloc);
            mesh->SetIndexAllocation(nullptr);
        }

        if (meshletAlloc)
        {
            m_MeshletAllocator->Free(meshletAlloc);
            mesh->SetMeshletAllocation(nullptr);
        }

        mesh->SetVertexAllocation(nullptr);
        mesh->SetIndexAllocation(nullptr);
        mesh->SetMeshletAllocation(nullptr);

        mesh->SetLoaded(false);
    }

    ME::Core::Memory::OAllocation MeshManager::VertexMalloc(SIZE_T size) 
    {
        Core::Memory::OAllocation alloc = nullptr;
        Core::Memory::BuddyAllocatorErrors result = m_VertexAllocator->Allocate(alloc, size);
        if (result != Core::Memory::BuddyAllocatorErrors::Success)
        {
            ME_ERROR("Can't allocate memory in vertex buffer! Error: {}", static_cast<uint32>(result));
            return nullptr;
        }
        return alloc;
    }

    void MeshManager::VertexFree(ME::Core::Memory::OAllocation& alloc) 
    {
        m_VertexAllocator->Free(alloc);
    }

    ME::Core::Memory::OAllocation MeshManager::IndexMalloc(SIZE_T size) 
    {
        Core::Memory::OAllocation alloc = nullptr;
        Core::Memory::BuddyAllocatorErrors result = m_IndexAllocator->Allocate(alloc, size);
        if (result != Core::Memory::BuddyAllocatorErrors::Success)
        {
            ME_ERROR("Can't allocate memory in index buffer! Error: {}", static_cast<uint32>(result));
            return nullptr;
        }
        return alloc;
    }

    void MeshManager::IndexFree(ME::Core::Memory::OAllocation& alloc) 
    {
        m_IndexAllocator->Free(alloc);
    }

    ME::Core::Memory::OAllocation MeshManager::MeshletMalloc(SIZE_T size)
    {
        Core::Memory::OAllocation alloc = nullptr;
        Core::Memory::BuddyAllocatorErrors result = m_MeshletAllocator->Allocate(alloc, size);
        if (result != Core::Memory::BuddyAllocatorErrors::Success)
            ME_WARN("Can't allocate memory in meshlet buffer! Error: {}", static_cast<uint32>(result));
        return alloc;
    }

    void MeshManager::MeshletFree(ME::Core::Memory::OAllocation& alloc)
    {
        m_MeshletAllocator->Free(alloc);
    }
}
