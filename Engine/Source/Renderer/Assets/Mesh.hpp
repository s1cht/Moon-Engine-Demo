#pragma once
#include <Core.hpp>
#include <Core/Math/Math.hpp>
#include <Core/Memory/Memory.hpp>
#include <Core/Containers/Array.hpp>
#include <Core/Containers/String.hpp>
#include <Core/Memory/Allocators/BuddyAllocator.hpp>

namespace ME::Assets
{
	struct Vertex
	{
		ME::Core::Math::Vector3D32 Position;
		ME::Core::Math::Vector3D32 Normal;
		ME::Core::Math::Vector4D32 Tangent;
		ME::Core::Math::Vector2D32 TextureCoordinates;

		inline bool operator==(const Vertex& other) const {
			return Position == other.Position &&
				TextureCoordinates == other.TextureCoordinates &&
				Normal == other.Normal;
		}
	};

	struct BoundingBox
	{
		ME::Core::Math::Vector3D32 CenterPosition;
		ME::Core::Math::Vector3D32 Extents;
	};

	struct BoundingSphere
	{
		ME::Core::Math::Vector3D32 CenterPosition;
		float32 Radius;
	};

	struct Meshlet
	{
		uint32 VertexOffset;
		uint32 IndexOffset;
		uint32 VertexCount;
		uint32 TriangleCount;
		uint32 MeshId;
		BoundingSphere BoundingSphere;
	    uint32 Padding[3];
	};

	struct DrawMeshData
	{
		uint32 VertexOffset;
		uint32 MeshletOffset;
		uint32 IndexOffset;

		uint32 MeshletCount;
		uint32 VertexCount;
	    uint32 IndexCount;

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
        void UpdateMeshInfo(const ME::Core::Array<Vertex>& vertices, 
			const ME::Core::Array<uint32>& indices);

        inline void SetGroupName(const ME::Core::String& groupName) { m_GroupName = groupName; }
		inline void SetGroupName(ME::Core::String&& groupName) { m_GroupName = groupName; }

        inline ME::Core::Array<Vertex>& GetVertices() { return m_Vertices; }
		inline ME::Core::Array<uint32>& GetIndices() { return m_Indices; }
		inline ME::Core::Array<Meshlet>& GetMeshlets() { return m_Meshlets; }
		inline ME::Core::StringView GetGroupName() const { return m_GroupName.ToStringView(); }

		inline void SetMeshBox(const BoundingBox& meshBox) { m_MeshBox = meshBox; }
		inline BoundingBox GetMeshBox() const { return m_MeshBox; }

		inline void SetDrawData(const DrawMeshData& drawData) { m_DrawData = drawData; }
	    inline DrawMeshData GetDrawData() const { return m_DrawData; }

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
			
	private:
		void GenerateMeshlets();
		void CalculateMeshBox();

	private:
		bool m_Loaded;

		uint64 m_MeshId;
	private:
		ME::Core::Memory::OAllocation m_VertexAllocation;
		ME::Core::Memory::OAllocation m_IndexAllocation;
		ME::Core::Memory::OAllocation m_MeshletAllocation;

		ME::Core::String m_GroupName;

	private:
		ME::Core::Array<Vertex> m_Vertices;
		ME::Core::Array<uint32> m_Indices;
		ME::Core::Array<Meshlet> m_Meshlets;
		BoundingBox m_MeshBox;
		DrawMeshData m_DrawData;

	public:
		static ME::Core::Memory::Reference<Mesh> Create();

	};
}

ME_FMT_FORMATTER(ME::Assets::Vertex, "Position: {}; TextureCoordinates: {}; Normal: {}", ME_FMT_FORMATTER_VALUE(Position), ME_FMT_FORMATTER_VALUE(TextureCoordinates), ME_FMT_FORMATTER_VALUE(Normal));