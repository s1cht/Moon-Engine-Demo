#include "Mesh.h"

namespace Pawn::Assets
{
	Pawn::Assets::Mesh::Mesh()
		: m_GroupName(TEXT("default"))
	{
		m_Vertexes = Pawn::Core::Containers::Array<Vertex>();
		m_Indexes = Pawn::Core::Containers::Array<int32>();
	}

	Pawn::Assets::Mesh::Mesh(const Pawn::Core::Containers::String& groupName)
		: m_GroupName(groupName)
	{
		m_Vertexes = Pawn::Core::Containers::Array<Vertex>();
		m_Indexes = Pawn::Core::Containers::Array<int32>();
	}

	Pawn::Assets::Mesh::~Mesh()
	{

	}

	Pawn::Core::Memory::Reference<Pawn::Render::VertexBuffer> Pawn::Assets::Mesh::GetVertexBuffer()
	{
		return m_VertexBuffer;
	}

	Pawn::Core::Memory::Reference<Pawn::Render::IndexBuffer> Pawn::Assets::Mesh::GetIndexBuffer()
	{
		return m_IndexBuffer;
	}

	void Mesh::SetVertexes(const Pawn::Core::Containers::Array<Vertex>& vertexes)
	{
		m_Vertexes = vertexes;
	}

	void Mesh::SetIndexes(const Pawn::Core::Containers::Array<int32>& indexes)
	{
		m_Indexes = indexes;
	}

	void Mesh::SetGroupName(const Pawn::Core::Containers::String& groupName)
	{
		m_GroupName = groupName;
	}

	void Mesh::SetVertexes(Pawn::Core::Containers::Array<Vertex>&& vertexes)
	{
		m_Vertexes = Pawn::Core::Containers::Array<Vertex>(std::move(vertexes));
	}

	void Mesh::SetIndexes(Pawn::Core::Containers::Array<int32>&& indexes)
	{
		m_Indexes = Pawn::Core::Containers::Array<int32>(std::move(indexes));
	}

	void Mesh::SetGroupName(Pawn::Core::Containers::String&& groupName)
	{
		m_GroupName = std::move(groupName);
	}

	void Mesh::CreateBuffers()
	{

	}

}