#include "Mesh.h"

namespace Pawn::Assets
{
	Pawn::Assets::Mesh::Mesh()
		: m_GroupName(TEXT("default"))
	{
	}

	Pawn::Assets::Mesh::Mesh(const Pawn::Core::Containers::String& groupName)
		: m_GroupName(groupName)
	{
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

	void Mesh::SetVertexes(Pawn::Core::Containers::Array<Vertex> vertexes)
	{
		m_Vertexes = vertexes;
	}

	void Mesh::SetIndexes(Pawn::Core::Containers::Array<int32> indexes)
	{
		m_Indexes = indexes;
	}

	void Mesh::SetGroupName(Pawn::Core::Containers::String groupName)
	{
		groupName = m_GroupName;
	}

	void Mesh::CreateBuffers()
	{

	}

}