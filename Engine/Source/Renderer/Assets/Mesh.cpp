#include "Mesh.hpp"

#include "Renderer/Managers/MeshManager.hpp"

namespace ME::Assets
{
	ME::Assets::Mesh::Mesh()
		: m_Loaded(false),
		m_MeshId(~0u),
		m_VertexAllocation(nullptr), m_IndexAllocation(nullptr),
		m_GroupName(TEXT("default")),
		m_Vertices({}), m_Indices({})
	{
	}

	ME::Assets::Mesh::Mesh(const ME::Core::String& groupName)
		: m_Loaded(false),
        m_MeshId(~0u),
        m_VertexAllocation(nullptr), m_IndexAllocation(nullptr),
        m_GroupName(groupName),
        m_Vertices({}), m_Indices({})
	{
	}

	ME::Assets::Mesh::~Mesh()
	{
		Render::Manager::MeshManager::Get().UnloadMesh(m_MeshId);
	}

    void Mesh::SetVertices(const ME::Core::Containers::Array<Vertex>& vertices)
	{
        if (!m_Loaded)
        {
            ME_WARN("Can't update vertices in the loaded mesh {0}! Using previous mesh.");
            return;
        }
	    m_Vertices = vertices;
	    CalculateMeshBox();
	    GenerateMeshlets();
	}

    void Mesh::SetVertices(ME::Core::Containers::Array<Vertex>&& vertices)
	{
        if (m_Loaded)
        {
            ME_WARN("Can't update vertices in the loaded mesh {0}! Using previous mesh.");
            return;
        }
	    m_Vertices = ME::Core::Containers::Array(std::move(vertices));
	    CalculateMeshBox();
	    GenerateMeshlets();
	}

    void Mesh::SetIndices(const ME::Core::Containers::Array<uint32>& indices)
	{
        if (m_Loaded)
        {
            ME_WARN("Can't update indices in the loaded mesh {0}! Using previous mesh.");
            return;
        }
	    m_Indices = indices;
	    GenerateMeshlets();
	}

    void Mesh::SetIndices(ME::Core::Containers::Array<uint32>&& indices)
	{
        if (m_Loaded)
        {
            ME_WARN("Can't update indices in the loaded mesh {0}! Using previous mesh.");
            return;
        }
	    m_Indices = ME::Core::Containers::Array(std::move(indices));
	    GenerateMeshlets();
	}

    void Mesh::SetMeshlets(const ME::Core::Containers::Array<Meshlet>& meshlets)
	{
        if (m_Loaded)
        {
            ME_WARN("Can't update meshlets in the loaded mesh {0}! Using previous mesh.");
            return;
        }
	    m_Meshlets = meshlets;
	}

    void Mesh::SetMeshlets(ME::Core::Containers::Array<Meshlet>&& meshlets)
	{
        if (m_Loaded)
        {
            ME_WARN("Can't update meshlets in the loaded mesh {0}! Using previous mesh.");
            return;
        }
	    m_Meshlets = ME::Core::Containers::Array(std::move(meshlets));
	}

    void Mesh::SetMeshBox(const MeshBox& meshBox)
	{
	    m_MeshBox = meshBox;
	}

    void Mesh::Load()
    {
		if (m_Loaded == true) return;
		if (!Render::Manager::MeshManager::Get().LoadMesh(m_MeshId))
		{
			ME_WARN("Mesh loading failed! Mesh will be skipped when rendering happens.");
			return;
		}
        m_DrawData.MeshID = static_cast<uint32>(m_MeshId);
		m_DrawData.command.firstIndex = static_cast<uint32>(m_IndexAllocation->Offset / sizeof(uint32));
		m_DrawData.command.indexCount = static_cast<uint32>(m_Indices.Size());
		m_DrawData.command.vertexOffset = static_cast<uint32>(m_VertexAllocation->Offset / sizeof(ME::Assets::Vertex));
		m_DrawData.command.firstInstance = 0;
		m_DrawData.command.instanceCount = m_UsageCount;
		m_Loaded = true;
    }

    void Mesh::Unload()
    {
		if (m_Loaded == false) return;
		Render::Manager::MeshManager::Get().UnloadMesh(m_MeshId);
		m_Loaded = false;
    }

    void Mesh::IncrementUsage()
	{
		++m_UsageCount;
	}

    void Mesh::DecrementUsage()
    {
		if (m_UsageCount <= 0) return;
		--m_UsageCount;	
    }

    void Mesh::GenerateMeshlets()
    {
        uint32 estimatedMeshlets = (m_Indices.Size() / 3 + ME_MESH_MNG_MESHLET_MAX_TRI_COUNT - 1) / ME_MESH_MNG_MESHLET_MAX_TRI_COUNT;

        m_Meshlets.Clear();
        m_Meshlets.Reserve(estimatedMeshlets);

        Core::Containers::Array<Vertex> newVertices;
        Core::Containers::Array<uint32> newIndices;

        newVertices.Reserve(m_Vertices.Size());
        newIndices.Reserve(m_Indices.Size());

        Core::Containers::Array<int32> remap;
        remap.Resize(m_Vertices.Size());
        for (uint32 r = 0; r < remap.Size(); ++r) remap[r] = -1;

        Core::Containers::Array<uint32> usedRemapIndices;

        uint32 indexCursor = 0;
        uint32 currentVertexOffset = 0;
        uint32 currentIndexOffset = 0;

        while (indexCursor < m_Indices.Size())
        {
            usedRemapIndices.Clear();
            Core::Containers::Array<Vertex> localVertices;
            Core::Containers::Array<uint32> localIndices;
            localVertices.Reserve(ME::Core::Algorithm::Min<uint32>(ME_MESH_MNG_MESHLET_MAX_VERTEX_COUNT, 16));
            localIndices.Reserve(ME::Core::Algorithm::Min<uint32>(ME_MESH_MNG_MESHLET_MAX_TRI_COUNT * 3, 64));

            uint32 triCount = 0;

            while (triCount < ME_MESH_MNG_MESHLET_MAX_TRI_COUNT && indexCursor + 2 < m_Indices.Size())
            {
                uint32 origA = m_Indices[indexCursor + 0];
                uint32 origB = m_Indices[indexCursor + 1];
                uint32 origC = m_Indices[indexCursor + 2];

                uint32 newVerts = 0;
                if (remap[origA] == -1) ++newVerts;
                if (remap[origB] == -1) ++newVerts;
                if (remap[origC] == -1) ++newVerts;

                if (localVertices.Size() + newVerts > ME_MESH_MNG_MESHLET_MAX_VERTEX_COUNT)
                    break;

                uint32 localIdxA, localIdxB, localIdxC;

                auto addVertexIfNeeded = [&](uint32 orig)->uint32
                {
                    int32 mapped = remap[orig];
                    if (mapped != -1) return static_cast<uint32>(mapped);

                    uint32 newLocal = localVertices.Size();
                    localVertices.PushBack(m_Vertices[orig]);
                    remap[orig] = static_cast<int32>(newLocal);
                    usedRemapIndices.PushBack(orig);     
                    return newLocal;
                };

                localIdxA = addVertexIfNeeded(origA);
                localIdxB = addVertexIfNeeded(origB);
                localIdxC = addVertexIfNeeded(origC);

                localIndices.PushBack(localIdxA);
                localIndices.PushBack(localIdxB);
                localIndices.PushBack(localIdxC);

                triCount++;
                indexCursor += 3;
            }

            if (triCount == 0)
                break;

            uint32 meshletVertexOffset = currentVertexOffset;
            uint32 meshletIndexOffset = currentIndexOffset;

            for (uint32 vi = 0; vi < localVertices.Size(); ++vi)
                newVertices.PushBack(std::move(localVertices[vi]));
            for (uint32 ii = 0; ii < localIndices.Size(); ++ii)
                newIndices.PushBack(localIndices[ii]);

            Meshlet meshlet = {};
            meshlet.VertexOffset = meshletVertexOffset;
            meshlet.IndexOffset = meshletIndexOffset;
            meshlet.VertexCount = localVertices.Size();
            meshlet.TriangleCount = triCount;
            meshlet.MeshId = m_MeshId;

            m_Meshlets.EmplaceBack(meshlet);

            currentVertexOffset += meshlet.VertexCount;
            currentIndexOffset += meshlet.TriangleCount * 3;

            for (uint32 orig : usedRemapIndices)
                remap[orig] = -1;
        }

        m_Vertices.Clear();
        m_Vertices.Append(newVertices);

        m_Indices.Clear();
        m_Indices.Append(newIndices);
    }

    void Mesh::CalculateMeshBox()
    {
		ME::Core::Math::Vector3D32 min = ME::Core::Math::Vector3D32(0);
		ME::Core::Math::Vector3D32 max = ME::Core::Math::Vector3D32(0);
        for (const auto& vertex : m_Vertices)
        {
            if (vertex.Position.X < min.X)
                min.x = vertex.Position.X;
            if (vertex.Position.y < min.Y)
                min.y = vertex.Position.Y;
            if (vertex.Position.z < min.Z)
                min.z = vertex.Position.Z;

            if (vertex.Position.x > max.X)
                max.x = vertex.Position.X;
            if (vertex.Position.y > max.Y)
                max.y = vertex.Position.Y;
            if (vertex.Position.z > max.Z)
                max.z = vertex.Position.Z;
        }

		m_MeshBox.Extents = (max - min) * 0.5f;
		m_MeshBox.CenterPosition = min + m_MeshBox.Extents;
    }

    ME::Core::Memory::Reference<Mesh> Mesh::Create()
    {
        return ME::Core::Memory::Reference<Mesh>(new Mesh());
    }
}
