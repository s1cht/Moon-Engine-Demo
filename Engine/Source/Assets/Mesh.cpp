#include "Mesh.h"

namespace ME::Assets
{
	ME::Assets::Mesh::Mesh()
		: m_GroupName(TEXT("default")), m_Valid(false)
	{
		m_Vertices = ME::Core::Containers::Array<Vertex>();
		m_Indices = ME::Core::Containers::Array<uint32>();
	}

	ME::Assets::Mesh::Mesh(const ME::Core::Containers::String& groupName)
		: m_GroupName(groupName), m_Valid(false)
	{
		m_Vertices = ME::Core::Containers::Array<Vertex>();
		m_Indices = ME::Core::Containers::Array<uint32>();
	}

	ME::Assets::Mesh::~Mesh()
	{
		if (m_VertexBuffer != nullptr)
		{
			m_VertexBuffer->Shutdown();
			m_VertexBuffer = nullptr;
		}
		if (m_IndexBuffer != nullptr)
		{
			m_IndexBuffer->Shutdown();
			m_IndexBuffer = nullptr;
		}
	}

	void Mesh::SetIndices(const ME::Core::Containers::Array<uint32>& indices)
	{
		m_Indices = indices;
	}

	void Mesh::SetVertices(const ME::Core::Containers::Array<Vertex>& vertices)
	{
		m_Vertices = vertices;
	}

	void Mesh::SetGroupName(ME::Core::Containers::String groupName)
	{
		m_GroupName = groupName;
	}

	void Mesh::SetVertices(ME::Core::Containers::Array<Vertex>&& vertices)
	{
		m_Vertices = ME::Core::Containers::Array<Vertex>(std::move(vertices));
	}

	void Mesh::SetIndices(ME::Core::Containers::Array<uint32>&& indexes)
	{
		m_Indices = ME::Core::Containers::Array<uint32>(std::move(indexes));
	}

	void Mesh::SetGroupName(ME::Core::Containers::String&& groupName)
	{
		m_GroupName = std::move(groupName);
	}

	void Mesh::Bind(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer)
	{
		m_VertexBuffer->Bind(commandBuffer);
		m_IndexBuffer->Bind(commandBuffer);
	}

	void Mesh::Unbind() const
	{
		m_VertexBuffer->Unbind();
		m_IndexBuffer->Unbind();
	}

	void Mesh::CreateBuffers()
	{
		Render::VertexBufferSpecification vbSpecification = {};
		vbSpecification.Size = m_Vertices.GetSize() * sizeof(ME::Assets::Vertex);
		vbSpecification.DebugName = m_GroupName;

		Render::IndexBufferSpecification ibSpecification = {};
		ibSpecification.IndexCount = static_cast<uint32>(m_Indices.GetSize());
		ibSpecification.DebugName = m_GroupName;

		if (m_VertexBuffer != nullptr)
			m_VertexBuffer->Shutdown();
		if (m_IndexBuffer != nullptr)
			m_IndexBuffer->Shutdown();

		m_VertexBuffer = nullptr;
		m_IndexBuffer = nullptr;

		m_VertexBuffer = ME::Render::VertexBuffer::Create(vbSpecification);
		m_IndexBuffer = ME::Render::IndexBuffer::Create(ibSpecification);

		m_Valid = false;
	}

	void Mesh::UpdateBuffers(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer)
	{
		if (m_Valid == true) return;
		m_VertexBuffer->SetData(commandBuffer, static_cast<void*>(m_Vertices.Data()), m_Vertices.GetSize() * sizeof(Vertex));
		m_IndexBuffer->SetData(commandBuffer, m_Indices.Data(), m_Indices.GetSize());
		m_Valid = true;
	}
}
