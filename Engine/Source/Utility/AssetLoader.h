//	Asset loader

//	Not finished
//	To be implemented in future: 
//		1. Material support. (.mtl)
//		2. .FBX file support. (Mesh, material, animation)
//

#pragma once

#include <Core.hpp>
#include <Core/Math/Math.hpp>
#include <Core/Memory/Memory.hpp>
#include <Core/Containers/Array.hpp>
#include <Core/Containers/String/String.hpp>
#include <Core/Utils/Logging/Logger.hpp>


namespace ME::Assets
{
	struct Vertex;
	class Mesh;
	class Image;
}

namespace ME::Utility
{
	struct IndexPack
	{
		int32 p;
		int32 uv;
		int32 n;

		bool operator==(const IndexPack& other) const
		{
			return p == other.p && uv == other.uv && n == other.n;
		}
	};

	enum class AssetFileFormats
	{
		None = 0,
		OBJ, 
		TRG, 
		// For future:
		// FBX,
	};

	struct AssetLoadResult
	{
		ME::Core::Containers::Array<ME::Core::Memory::Reference<Assets::Mesh>> Meshes;
		ME::Core::Containers::Array<ME::Core::Memory::Reference<Assets::Image>> Images;

		// For future
		//Array<Memory::Scope<Assets::Material>> Materials;
		//Array<Memory::Scope<Assets::Animation>> Animations;
	};


	class MEAPI AssetLoader
	{
	private:
#pragma pack(push, 1)
		// 18 bytes
		struct TRGHeader
		{
			// Field 1 (1 byte)
			uint8 idLength;
			// Field 2 (1 byte)
			uint8 colorMap;
			// Field 3 (1 byte)
			uint8 imageType;
			// Field 4 (5 byte)
			uint16 entryIndex;
			uint16 colorMapLength;
			uint8 colorMapEntrySize;
			// Field 5 (10 byte)
			uint16 xOrigin;
			uint16 yOrigin;
			uint16 width;
			uint16 height;
			uint8 pixelDepth;
			uint8 descriptor;
		};
#pragma pack(pop)

	public:
		static AssetLoadResult Load(const ME::Core::String& filePath, bool centered, AssetFileFormats format);

	private:
		static AssetLoadResult LoadOBJ(const char8* filePath, bool centered);
	private:
		static AssetLoadResult LoadTGA(const char8* filePath);
		// For future
		//static void LoadFBX(const char8* filePath);
	private:
		static ME::Core::Math::Vector3D32 CalculateNormal(const ME::Core::Containers::Array<Assets::Vertex>& vertices);
		static void ProjectTo2D(const ME::Core::Containers::Array<Assets::Vertex>& vertices, ME::Core::Containers::Array<ME::Core::Math::Vector2D32>& out2D);
		static bool IsPointInTriangle(const ME::Core::Math::Vector2D32& pt, const ME::Core::Math::Vector2D32& a, const ME::Core::Math::Vector2D32& b, const ME::Core::Math::Vector2D32& c);
		static bool IsEar(int i, const ME::Core::Containers::Array<ME::Core::Math::Vector2D32>& poly, const ME::Core::Containers::Array<uint32>& idx);
		static void Triangulate(const ME::Core::Containers::Array<Assets::Vertex>& vertices, ME::Core::Containers::Array<uint32>& indicesOut);
		static void Triangulate(const ME::Core::Containers::Array<uint32>& faceIndices, ME::Core::Containers::Array<uint32>& indicesOut);

	};
}