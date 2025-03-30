//	Asset loader

//	Not finished
//	To be implemented in future: 
//		1. Material support. (.mtl)
//		2. .FBX file support. (Mesh, material, animation)
//

#pragma once

#include <Core.h>

#include "Assets/Mesh.h"

import Pawn.Core.Memory;
import Pawn.Core.Container.Array;
import Pawn.Core.Container.String;

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
	public:
		static AssetLoadResult Load(const Pawn::Core::Containers::String& filePath, AssetFileFormats format = AssetFileFormats::OBJ);

	private:
		static AssetLoadResult LoadOBJ(const uchar* filePath);
		// For future
		//static void LoadFBX(const uchar* filePath);

	};
}


