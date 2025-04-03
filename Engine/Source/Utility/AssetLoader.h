//	Asset loader

//	Not finished
//	To be implemented in future: 
//		1. Material support. (.mtl)
//		2. .FBX file support. (Mesh, material, animation)
//

#pragma once

#include "Assets/Mesh.h"
#include <Core/Utils/Logging/Logger.h>
#include <type_traits>

import Pawn.Core.Memory;
import Pawn.Core.Container.Array;
import Pawn.Core.Container.String;

namespace Pawn::Assets
{
	struct Vertex;
	class Mesh;
}

namespace Pawn::Utility
{
	enum class AssetFileFormats
	{
		None = 0,
		OBJ, 
		// For future:
		// FBX,
	};

	struct AssetLoadResult
	{
		Pawn::Core::Containers::Array<Pawn::Core::Memory::Scope<Assets::Mesh>> Meshes;

		// For future
		//Array<Memory::Scope<Assets::Material>> Materials;
		//Array<Memory::Scope<Assets::Animation>> Animations;
	};

	struct VertexKey
	{
		int32 positionIdx;
		int32 uvIdx;
		int32 normalIdx;

		VertexKey() : positionIdx(0), uvIdx(0), normalIdx(0) {}
		VertexKey(int32 p, int32 t, int32 n) : positionIdx(p), uvIdx(t), normalIdx(n) {}

		VertexKey& operator=(const VertexKey& other)
		{
			positionIdx = other.positionIdx;
			uvIdx = other.uvIdx;
			normalIdx = other.normalIdx;
		}

		bool operator==(const VertexKey& other) const
		{
			return positionIdx == other.positionIdx && uvIdx == other.uvIdx && normalIdx == other.normalIdx;
		}
	};

	struct VertexKeyHash
	{
		SIZE_T operator()(const VertexKey& key, SIZE_T) const
		{
			SIZE_T h1 = std::hash<int32>{}(key.positionIdx);
			SIZE_T h2 = std::hash<int32>{}(key.uvIdx);
			SIZE_T h3 = std::hash<int32>{}(key.normalIdx);
			return h1 ^ (h2 << 1) ^ (h3 << 2);
		}
	};

	class PAWN_API AssetLoader
	{
	private:
		struct VertexEntry 
		{
			VertexKey key;
			uint32 index;
			VertexEntry(const VertexKey& k, uint32 idx) : key(k), index(idx) {}
		};

		struct GroupEntry 
		{
			Pawn::Core::Containers::String Name;
			Pawn::Core::Containers::Array<int32> GroupIndices;
			Pawn::Core::Containers::String Material;

			GroupEntry(const Pawn::Core::Containers::String& n) : Name(n) {}
		};
	
	public:
		static AssetLoadResult Load(const Pawn::Core::Containers::String& filePath, AssetFileFormats format = AssetFileFormats::OBJ);

	private:
		static AssetLoadResult LoadOBJ(const uchar* filePath);
		// For future
		//static void LoadFBX(const uchar* filePath);

	};
}


