#include "Mesh.h"

namespace ME::Assets
{
	ME::Assets::Mesh::Mesh()
		: m_GroupName(TEXT("default"))
	{
		m_Vertexes = ME::Core::Containers::Array<Vertex>();
		m_Indexes = ME::Core::Containers::Array<int32>();
	}

	ME::Assets::Mesh::Mesh(const ME::Core::Containers::String& groupName)
		: m_GroupName(groupName)
	{
		m_Vertexes = ME::Core::Containers::Array<Vertex>();
		m_Indexes = ME::Core::Containers::Array<int32>();
	}

	ME::Assets::Mesh::~Mesh()
	{

	}

	ME::Core::Memory::Reference<ME::Render::VertexBuffer> ME::Assets::Mesh::GetVertexBuffer()
	{
		return m_VertexBuffer;
	}

	ME::Core::Memory::Reference<ME::Render::IndexBuffer> ME::Assets::Mesh::GetIndexBuffer()
	{
		return m_IndexBuffer;
	}

	ME::Core::Containers::Array<Vertex>& Mesh::GetVertices()
	{
		return m_Vertexes;
	}

	ME::Core::Containers::Array<int32>& Mesh::GetIndices()
	{
		return m_Indexes;
	}

	void Mesh::SetVertexes(const ME::Core::Containers::Array<Vertex>& vertexes)
	{
		m_Vertexes = vertexes;
	}

	void Mesh::SetIndexes(const ME::Core::Containers::Array<int32>& indexes)
	{
		m_Indexes = indexes;
	}

	void Mesh::SetGroupName(ME::Core::Containers::String groupName)
	{
		m_GroupName = groupName;
	}

	void Mesh::SetVertexes(ME::Core::Containers::Array<Vertex>&& vertexes)
	{
		m_Vertexes = ME::Core::Containers::Array<Vertex>(std::move(vertexes));
	}

	void Mesh::SetIndexes(ME::Core::Containers::Array<int32>&& indexes)
	{
		m_Indexes = ME::Core::Containers::Array<int32>(std::move(indexes));
	}

	void Mesh::SetGroupName(ME::Core::Containers::String&& groupName)
	{
		m_GroupName = std::move(groupName);
	}

	void Mesh::CreateBuffers()
	{
		m_VertexBuffer.reset(ME::Render::VertexBuffer::Create(const_cast<Vertex*>(m_Vertexes.Data()), sizeof(ME::Assets::Vertex) * m_Vertexes.GetSize(), ME::Render::Usage::Default));
		m_IndexBuffer.reset(ME::Render::IndexBuffer::Create(const_cast<int32*>(m_Indexes.Data()), (uint32)m_Indexes.GetSize(), ME::Render::Usage::Default));
	}

}