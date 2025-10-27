#include "MeshManager.hpp"

#include "Renderer/RenderCommand.h"

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
        Core::Memory::BuddyAllocatorSpecification vertexSpecs = {};
        vertexSpecs.MinBlockSize = ME_MESH_MNG_MIN_SIZE;
        vertexSpecs.TotalSize = info.VertexMemoryPoolSize;

        Core::Memory::BuddyAllocatorSpecification indexSpecs = {};
        indexSpecs.MinBlockSize = ME_MESH_MNG_MIN_SIZE;
        indexSpecs.TotalSize = info.IndexMemoryPoolSize;

        Core::Memory::BuddyAllocatorSpecification meshletSpecs = {};
        meshletSpecs.MinBlockSize = ME_MESH_MNG_MIN_SIZE;
        meshletSpecs.TotalSize = info.MeshletMemoryPoolSize;


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

        ResourceBinding commonBinding = {};
        commonBinding.MaxSize = 1;
        commonBinding.Stage = ShaderStage::Vertex | ShaderStage::Mesh | ShaderStage::Task;
        commonBinding.Type = ResourceType::StorageBuffer;

        m_SetLayout = ResourceLayout(5);
        m_SetLayout.Clear();

        // Vertices
        m_SetLayout.EmplaceBack(commonBinding);
        // Indices
        m_SetLayout.EmplaceBack(commonBinding);
        // Meshlets
        m_SetLayout.EmplaceBack(commonBinding);
        // Mesh boxes
        m_SetLayout.EmplaceBack(commonBinding);
        // Draw info
        m_SetLayout.EmplaceBack(commonBinding);

        uint32 setIndex = Render::RenderCommand::GetResourceHandler()->CreateResourceSet(m_SetLayout);

        VertexBufferSpecification vertexMemPoolSpecs = {};
        vertexMemPoolSpecs.Set = 0;
        vertexMemPoolSpecs.Binding = 0;
        vertexMemPoolSpecs.ResourceBinding = commonBinding;
        vertexMemPoolSpecs.SetIndex = setIndex;
        vertexMemPoolSpecs.Size = info.VertexMemoryPoolSize;
        vertexMemPoolSpecs.DebugName = "Vertex memory pool";

        IndexBufferSpecification indexMemPoolSpecs = {};
        vertexMemPoolSpecs.Set = 0;
        vertexMemPoolSpecs.Binding = 1;
        vertexMemPoolSpecs.ResourceBinding = commonBinding;
        vertexMemPoolSpecs.SetIndex = setIndex;
        indexMemPoolSpecs.IndexCount = info.IndexMemoryPoolSize / 4;
        indexMemPoolSpecs.DebugName = "Index memory pool";

        StorageBufferSpecification meshletBufferSpecs = {};
        vertexMemPoolSpecs.Set = 0;
        vertexMemPoolSpecs.Binding = 2;
        vertexMemPoolSpecs.ResourceBinding = commonBinding;
        vertexMemPoolSpecs.SetIndex = setIndex;
        vertexMemPoolSpecs.Size = sizeof(ME::Assets::Meshlet) * ME_MESH_MNG_MESHLET_BUFFER_SIZE;
        meshletBufferSpecs.MemoryType = MemoryType::VRAM;
        meshletBufferSpecs.DebugName = "Meshlet buffer";

        StorageBufferSpecification meshBoxSpecs = {};
        meshBoxSpecs.Set = 0;
        meshBoxSpecs.Binding = 3;
        meshBoxSpecs.ResourceBinding = commonBinding;
        meshBoxSpecs.SetIndex = setIndex;
        meshBoxSpecs.Size = sizeof(ME::Assets::MeshBox) * ME_MESH_MNG_MAX_MESH_COUNT;
        meshBoxSpecs.MemoryType = MemoryType::VRAM;
        meshBoxSpecs.DebugName = "Mesh box buffer";

        StorageBufferSpecification drawInputSpecs = {};
        drawInputSpecs.Set = 0;
        drawInputSpecs.Binding = 4;
        drawInputSpecs.ResourceBinding = commonBinding;
        drawInputSpecs.SetIndex = setIndex;
        drawInputSpecs.Size = sizeof(ME::Assets::DrawIndirectIndexedMesh) * ME_MESH_MNG_MAX_MESH_COUNT;
        drawInputSpecs.MemoryType = MemoryType::VRAM;
        drawInputSpecs.DebugName = "Draw input buffer";

        m_VertexBuffer = ME::Render::VertexBuffer::Create(vertexMemPoolSpecs);
        m_IndexBuffer = ME::Render::IndexBuffer::Create(indexMemPoolSpecs);

        m_MeshBoxes = ME::Core::Containers::Array<ME::Assets::MeshBox>();
        m_MeshBoxes.Reserve(ME_MESH_MNG_MIN_SIZE);

        m_DrawData = ME::Core::Containers::Array<ME::Assets::DrawIndirectIndexedMesh>();
        m_DrawData.Reserve(ME_MESH_MNG_MIN_SIZE);
        
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
        Core::Memory::OAllocation vertexAlloc = VertexMalloc(mesh->GetVertices().Size());
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

        Core::Memory::OAllocation meshletAlloc = MeshletMalloc(mesh->GetMeshlets().Size() * sizeof(ME::Assets::Meshlet));
        if (meshletAlloc == nullptr)
        {
            MeshletFree(meshletAlloc);
            return false;
        }

        mesh->SetVertexAllocation(vertexAlloc);
        mesh->SetIndexAllocation(indexAlloc);
        mesh->SetMeshletAllocation(meshletAlloc);

        m_VertexBuffer->SetData(mesh->GetVertices().Data(), vertexAlloc->Size, vertexAlloc->Offset);
        m_IndexBuffer->SetData(mesh->GetIndices().Data(), mesh->GetIndices().Size(), indexAlloc->Offset / sizeof(uint32));
        m_MeshletBuffer->SetData(mesh->GetMeshlets().Data(), mesh->GetMeshlets().Size() * sizeof(ME::Assets::Meshlet), meshletAlloc->Offset);

        ME::Assets::DrawIndirectIndexedMesh drawData = mesh->GetDrawData();
        SIZE_T drawOffset = m_DrawData.Size() * sizeof(ME::Assets::DrawIndirectIndexedMesh);
        m_DrawData.EmplaceBack(drawData);
        m_DrawBuffer->SetData(&drawData, sizeof(ME::Assets::DrawIndirectIndexedMesh), drawOffset);

        ME::Assets::MeshBox meshBox = mesh->GetMeshBox();
        SIZE_T meshBoxOffset = m_MeshBoxes.Size() * sizeof(ME::Assets::MeshBox);
        m_MeshBoxes.EmplaceBack(meshBox);
        m_MeshBoxBuffer->SetData(&meshBox, sizeof(ME::Assets::MeshBox), meshBoxOffset);

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

        if (indexAlloc)
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
        {
            ME_ERROR("Can't allocate memory in meshlet buffer! Error: {}", static_cast<uint32>(result));
            return nullptr;
        }
        return alloc;
    }

    void MeshManager::MeshletFree(ME::Core::Memory::OAllocation& alloc)
    {
        m_MeshletAllocator->Free(alloc);
    }
}
