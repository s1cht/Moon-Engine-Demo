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
		Pawn::Core::Containers::Array<Pawn::Core::Memory::Reference<Assets::Mesh>> Meshes;

		// For future
		//Array<Memory::Scope<Assets::Material>> Materials;
		//Array<Memory::Scope<Assets::Animation>> Animations;
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


