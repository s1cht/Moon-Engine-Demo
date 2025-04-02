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
		float32 x, y, z;
		float32 uvU, uvV, uvW;
		float32 normalX, normalY, normalZ;
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
		void SetVertexes(Pawn::Core::Containers::Array<Vertex> vertexes);
		void SetIndexes(Pawn::Core::Containers::Array<int32> indexes);
		void SetGroupName(Pawn::Core::Containers::String groupName);

		void CreateBuffers();

	private:
		Pawn::Core::Containers::String m_GroupName;

		Pawn::Core::Containers::Array<Vertex> m_Vertexes;
		Pawn::Core::Containers::Array<int32> m_Indexes;

		Pawn::Core::Memory::Reference<Render::VertexBuffer> m_VertexBuffer;
		Pawn::Core::Memory::Reference<Render::IndexBuffer> m_IndexBuffer;

	};
}

