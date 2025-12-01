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
#include <Core/Containers/String.hpp>

#include "Assets/AudioFile.hpp"

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
		WAV,
		// For future:
		// FBX,
	};

	struct AssetLoadResult
	{
		ME::Core::Array<ME::Core::Memory::Reference<Assets::Mesh>> Meshes;
		ME::Core::Array<ME::Core::Memory::Reference<Assets::Image>> Images;
		Assets::AudioFile Audio;

		// For future
		//Array<Memory::Scope<Assets::Material>> Materials;
		//Array<Memory::Scope<Assets::Animation>> Animations;
	};


	class MEAPI AssetLoader
	{
	private:
		// 18 bytes
        #pragma pack(push, 1)
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
        #pragma pack(push, 1)
	    struct WAVHeader
		{
			// RIFF chunk
			uint32 FileSize;
			uint32 FileFormat;

			// fmt chunk
			uint16 AudioFormat;
			uint16 NbrChannels;
			uint32 Frequency;
			uint32 BytePerSec;
			uint16 BytePerBloc;
			uint16 BitsPerSample;
			uint16 cbSize;
			uint16 wValidBitsPerSample;
			uint32 dwChannelMask;
			uint8 SubFormat[16];

			// data chunk
			uint32 DataSize;
			uint8* Data;

	        // LIST chunk (ignored)
	        // INFO chunk (ignored)
		};
        #pragma pack(pop)

	public:
		static AssetLoadResult Load(const ME::Core::String& filePath, bool centered, AssetFileFormats format);

	private:
		// Meshes
		static AssetLoadResult LoadOBJ(const char8* filePath, bool centered);
	private:
		// Images
		static AssetLoadResult LoadTGA(const char8* filePath);
		// For future
		//static void LoadFBX(const char8* filePath);
	private:
		static AssetLoadResult LoadWAV(const char8* filePath);

	private:
		static ME::Core::Math::Vector3D32 CalculateNormal(const ME::Core::Array<Assets::Vertex>& vertices);
		static void ProjectTo2D(const ME::Core::Array<Assets::Vertex>& vertices, ME::Core::Array<ME::Core::Math::Vector2D32>& out2D);
		static bool IsPointInTriangle(const ME::Core::Math::Vector2D32& pt, const ME::Core::Math::Vector2D32& a, const ME::Core::Math::Vector2D32& b, const ME::Core::Math::Vector2D32& c);
		static bool IsEar(int i, const ME::Core::Array<ME::Core::Math::Vector2D32>& poly, const ME::Core::Array<uint32>& idx);
		static void Triangulate(const ME::Core::Array<Assets::Vertex>& vertices, ME::Core::Array<uint32>& indicesOut);
	};
}