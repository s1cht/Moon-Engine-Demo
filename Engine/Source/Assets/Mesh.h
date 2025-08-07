#pragma once

#include "Renderer/Base/Buffer.h"

#include <Core.hpp>
#include <Core/Math/Math.hpp>
#include <Core/Memory/Memory.hpp>
#include <Core/Containers/Array.hpp>
#include <Core/Containers/String/String.hpp>

namespace ME::Assets
{
	struct Vertex
	{
		ME::Core::Math::Vector3D32 Position;
		ME::Core::Math::Vector2D32 TextureCoordinate;
		ME::Core::Math::Vector3D32 Normal;
	};

	class MOON_API Mesh
	{
	public: 
		Mesh();
		Mesh(const ME::Core::Containers::String& path);

		~Mesh();

	public:
		ME::Core::Memory::Reference<Render::VertexBuffer> GetVertexBuffer();
		ME::Core::Memory::Reference<Render::IndexBuffer> GetIndexBuffer();
		ME::Core::Containers::Array<Vertex>& GetVertices();
		ME::Core::Containers::Array<int32>& GetIndices();
		const ME::Core::Containers::StringView GetGroupName() const
		{
			return m_GroupName.ToStringView();
		}

	public:
		void SetVertexes(const ME::Core::Containers::Array<Vertex>& vertexes);
		void SetIndexes(const ME::Core::Containers::Array<int32>& indexes);
		void SetGroupName(ME::Core::Containers::String groupName);

		void SetVertexes(ME::Core::Containers::Array<Vertex>&& vertexes);
		void SetIndexes(ME::Core::Containers::Array<int32>&& indexes);
		void SetGroupName(ME::Core::Containers::String&& groupName);

		void CreateBuffers();

	private:
		ME::Core::Containers::String m_GroupName;

		ME::Core::Containers::Array<Vertex> m_Vertexes;
		ME::Core::Containers::Array<int32> m_Indexes;

		ME::Core::Memory::Reference<Render::VertexBuffer> m_VertexBuffer;
		ME::Core::Memory::Reference<Render::IndexBuffer> m_IndexBuffer;

	};
}

