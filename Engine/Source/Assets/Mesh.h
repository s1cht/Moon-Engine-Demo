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
		ME::Core::Math::Vector2D32 TextureCoords;
		ME::Core::Math::Vector3D32 Normal;

		inline bool operator==(const Vertex& other) const {
			return Position == other.Position &&
				TextureCoords == other.TextureCoords &&
				Normal == other.Normal;
		}
	};

	class MOON_API Mesh
	{
	public: 
		Mesh();
		Mesh(const ME::Core::Containers::String& path);

		~Mesh();

	public:
		ME::Core::Memory::Reference<Render::VertexBuffer> GetVertexBuffer() const
		{
			return m_VertexBuffer;
		}

		ME::Core::Memory::Reference<Render::IndexBuffer> GetIndexBuffer() const
		{
			return m_IndexBuffer;
		}

		ME::Core::Containers::Array<Vertex>& GetVertices()
		{
			return m_Vertices;
		}

		ME::Core::Containers::Array<uint32>& GetIndices()
		{
			return m_Indices;
		}

		const ME::Core::Containers::StringView GetGroupName() const
		{
			return m_GroupName.ToStringView();
		}

		bool IsValid() const { return m_Valid; }

	public:
		void SetVertices(const ME::Core::Containers::Array<Vertex>& vertexes);
		void SetIndices(const ME::Core::Containers::Array<uint32>& indexes);
		void SetGroupName(ME::Core::Containers::String groupName);

		void SetVertices(ME::Core::Containers::Array<Vertex>&& vertexes);
		void SetIndices(ME::Core::Containers::Array<uint32>&& indexes);
		void SetGroupName(ME::Core::Containers::String&& groupName);

	public:
		void Bind(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer);
		void Unbind() const;
		void CreateBuffers();
		void UpdateBuffers(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer);

	private:
		bool m_Valid;

		ME::Core::Containers::String m_GroupName;

		ME::Core::Containers::Array<Vertex> m_Vertices;
		ME::Core::Containers::Array<uint32> m_Indices;

		ME::Core::Memory::Reference<Render::VertexBuffer> m_VertexBuffer;
		ME::Core::Memory::Reference<Render::IndexBuffer> m_IndexBuffer;

	};
}

ME_FMT_FORMATTER(ME::Assets::Vertex, "Position: {}; TextureCoords: {}; Normal: {}", ME_FMT_FORMATTER_VALUE(Position), ME_FMT_FORMATTER_VALUE(TextureCoords), ME_FMT_FORMATTER_VALUE(Normal));