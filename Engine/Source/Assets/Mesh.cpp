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
}