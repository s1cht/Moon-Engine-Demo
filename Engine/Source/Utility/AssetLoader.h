//	Asset loader

//	Not finished
//	To be implemented in future: 
//		1. Material support. (.mtl)
//		2. .FBX file support. (Mesh, material, animation)
//

#pragma once

#include "Assets/Mesh.h"
#include <Core/Utils/Logging/Logger.h>

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

	struct VertexKey {
		int32 p, t, n;
		VertexKey(int32 pos, int32 tex, int32 norm) : p(pos), t(tex), n(norm) {}
		bool operator==(const VertexKey& other) const 
		{
			return p == other.p && t == other.t && n == other.n;
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


