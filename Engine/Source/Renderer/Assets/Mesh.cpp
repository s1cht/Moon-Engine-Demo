#include "Mesh.hpp"

#include <algorithm>

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

    void Mesh::UpdateMeshInfo(const ME::Core::Array<Vertex>& vertices,
        const ME::Core::Array<uint32>& indices)
    {
        if (m_Loaded)
        {
            ME_WARN("Can't update vertices in the loaded mesh {0}! Using previous mesh.", m_GroupName);
            return;
        }
	    m_Indices = indices;
	    m_Vertices = vertices;
	    CalculateMeshBox();
	    GenerateMeshlets();
    }

    void Mesh::Load()
    {
		if (m_Loaded == true) return;

        m_DrawData.MeshID = static_cast<uint32>(m_MeshId);

	    if (!Render::Manager::MeshManager::Get().LoadMesh(m_MeshId))
		{
			ME_WARN("Mesh loading failed! Mesh will be skipped when rendering happens.");
			return;
		}

		m_Loaded = true;
    }

    void Mesh::Unload()
    {
		if (m_Loaded == false) return;
		Render::Manager::MeshManager::Get().UnloadMesh(m_MeshId);
		m_Loaded = false;
    }

    void Mesh::GenerateMeshlets()
    {
        uint32 estimatedMeshlets = static_cast<uint32>((m_Indices.Size() / 3 + ME_MESH_MNG_MESHLET_MAX_TRI_COUNT - 1) / ME_MESH_MNG_MESHLET_MAX_TRI_COUNT);

        m_Meshlets.Clear();

        uint32 indexCursor = 0;
        uint32 currentVertexOffset = 0;
        uint32 currentIndexOffset = 0;
    }

    void Mesh::CalculateMeshBox()
    {
		ME::Core::Math::Vector3D32 min = ME::Core::Math::Vector3D32(0.f);
		ME::Core::Math::Vector3D32 max = ME::Core::Math::Vector3D32(0.f);
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
        return ME::Core::Memory::MakeReference<Mesh>();
    }
}
