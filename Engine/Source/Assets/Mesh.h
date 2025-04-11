#pragma once

#include "Renderer/Base/Buffer.h"

#include <Core.h>

import Pawn.Core.Math;
import Pawn.Core.Memory;
import Pawn.Core.Container.Array;
import Pawn.Core.Container.String;
import Pawn.Core.Container.StringView;

namespace Pawn::Assets
{
	struct Vertex
	{
		Pawn::Core::Math::Vector3D32 Position;
		Pawn::Core::Math::Vector2D32 TextureCoordinate;
		Pawn::Core::Math::Vector3D32 Normal;
	};

	class PAWN_API Mesh
	{
	public: 
		Mesh();
		Mesh(const Pawn::Core::Containers::String& path);

		~Mesh();

	public:
		Pawn::Core::Memory::Reference<Render::VertexBuffer> GetVertexBuffer();
		Pawn::Core::Memory::Reference<Render::IndexBuffer> GetIndexBuffer();
		const Pawn::Core::Containers::StringView GetGroupName() const
		{
			return m_GroupName.ToStringView();
		}

	public:
		void SetVertexes(const Pawn::Core::Containers::Array<Vertex>& vertexes);
		void SetIndexes(const Pawn::Core::Containers::Array<int32>& indexes);
		void SetGroupName(const Pawn::Core::Containers::String& groupName);

		void SetVertexes(Pawn::Core::Containers::Array<Vertex>&& vertexes);
		void SetIndexes(Pawn::Core::Containers::Array<int32>&& indexes);
		void SetGroupName(Pawn::Core::Containers::String&& groupName);

		void CreateBuffers();

	private:
		Pawn::Core::Containers::String m_GroupName;

		Pawn::Core::Containers::Array<Vertex> m_Vertexes;
		Pawn::Core::Containers::Array<int32> m_Indexes;

		Pawn::Core::Memory::Reference<Render::VertexBuffer> m_VertexBuffer;
		Pawn::Core::Memory::Reference<Render::IndexBuffer> m_IndexBuffer;

	};
}

