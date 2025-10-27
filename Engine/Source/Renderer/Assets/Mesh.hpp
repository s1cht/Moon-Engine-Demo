#pragma once

#include <Core.hpp>
#include <Core/Math/Math.hpp>
#include <Core/Memory/Memory.hpp>
#include <Core/Containers/Array.hpp>
#include <Core/Containers/String/String.hpp>
#include <Core/Memory/Allocators/BuddyAllocator.hpp>

#include "Renderer/Base/RenderAPI.h"

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

	struct MeshBox
	{
		ME::Core::Math::Vector3D32 CenterPosition;
		ME::Core::Math::Vector3D32 Extents;
	};

	struct Meshlet
	{
		uint32 VertexOffset;
		uint32 IndexOffset;
		uint32 VertexCount;
		uint32 TriangleCount;
		uint32 MeshId;
		uint32 Padding[3];
	};

	struct DrawIndirectIndexedMesh
	{
		ME::Render::DrawIndirectIndexedData command;
		uint32 MeshID;
	};

	class MEAPI Mesh
	{
	public: 
		Mesh();
        explicit Mesh(const ME::Core::String& path);
		Mesh(const Mesh&) = delete;
		Mesh(Mesh&&) = delete;
		~Mesh();

	public:
        void SetVertices(const ME::Core::Containers::Array<Vertex>& vertices);
        void SetVertices(ME::Core::Containers::Array<Vertex>&& vertices);

        void SetIndices(const ME::Core::Containers::Array<uint32>& indices);
        void SetIndices(ME::Core::Containers::Array<uint32>&& indices);

        void SetMeshlets(const ME::Core::Containers::Array<Meshlet>& meshlets);
        void SetMeshlets(ME::Core::Containers::Array<Meshlet>&& meshlets);

        inline void SetGroupName(const ME::Core::String& groupName) { m_GroupName = groupName; }
		inline void SetGroupName(ME::Core::String&& groupName) { m_GroupName = groupName; }

        void SetMeshBox(const MeshBox& meshBox);

        inline ME::Core::Containers::Array<Vertex>& GetVertices() { return m_Vertices; }
		inline ME::Core::Containers::Array<uint32>& GetIndices() { return m_Indices; }
		inline ME::Core::Containers::Array<Meshlet>& GetMeshlets() { return m_Meshlets; }
		inline ME::Core::StringView GetGroupName() const { return m_GroupName.ToStringView(); }
		inline MeshBox GetMeshBox() const { return m_MeshBox; }
		inline DrawIndirectIndexedMesh GetDrawData() const { return m_DrawData; }

		inline void SetVertexAllocation(const ME::Core::Memory::OAllocation& alloc) { m_VertexAllocation = alloc; }
		inline ME::Core::Memory::OAllocation GetVertexAllocation() const { return m_VertexAllocation; }

	    inline void SetIndexAllocation(const ME::Core::Memory::OAllocation& alloc) { m_IndexAllocation = alloc; }
		inline ME::Core::Memory::OAllocation GetIndexAllocation() const { return m_IndexAllocation; }

		inline void SetMeshletAllocation(const ME::Core::Memory::OAllocation& alloc) { m_MeshletAllocation = alloc; }
		inline ME::Core::Memory::OAllocation GetMeshletAllocation() const { return m_MeshletAllocation; }

		inline void SetMeshID(uint64 meshId) { m_MeshId = meshId; }
		inline uint64 GetMeshID() const { return m_MeshId; }

		inline void SetLoaded(bool loaded) { m_Loaded = loaded; }
		inline bool IsLoaded() const { return m_Loaded; }

	public:
		void Load();
		void Unload();

		void IncrementUsage();
        void DecrementUsage();
			
	private:
		void GenerateMeshlets();
		void CalculateMeshBox();

	private:
		bool m_Loaded;

		uint64 m_MeshId;
		std::atomic<uint32> m_UsageCount;

	private:
		ME::Core::Memory::OAllocation m_VertexAllocation;
		ME::Core::Memory::OAllocation m_IndexAllocation;
		ME::Core::Memory::OAllocation m_MeshletAllocation;

		ME::Core::String m_GroupName;

	private:
		ME::Core::Containers::Array<Vertex> m_Vertices;
		ME::Core::Containers::Array<uint32> m_Indices;
		ME::Core::Containers::Array<Meshlet> m_Meshlets;
		MeshBox m_MeshBox;
		DrawIndirectIndexedMesh m_DrawData;

	public:
		static ME::Core::Memory::Reference<Mesh> Create();

	};
}

ME_FMT_FORMATTER(ME::Assets::Vertex, "Position: {}; TextureCoords: {}; Normal: {}", ME_FMT_FORMATTER_VALUE(Position), ME_FMT_FORMATTER_VALUE(TextureCoords), ME_FMT_FORMATTER_VALUE(Normal));