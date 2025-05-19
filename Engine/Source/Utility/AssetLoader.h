//	Asset loader

//	Not finished
//	To be implemented in future: 
//		1. Material support. (.mtl)
//		2. .FBX file support. (Mesh, material, animation)
//

#pragma once

#include "Assets/Mesh.h"

#include <Core.hpp>
#include <Core/Memory/Memory.hpp>
#include <Core/Containers/Array.hpp>
#include <Core/Containers/String/String.hpp>
#include <Core/Utils/Logging/Logger.hpp>
#include <type_traits>

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
		static AssetLoadResult Load(const Pawn::Core::Containers::String& filePath, bool centered = false, AssetFileFormats format = AssetFileFormats::OBJ);

	private:
		static AssetLoadResult LoadOBJ(const uchar* filePath, bool centered);
		// For future
		//static void LoadFBX(const uchar* filePath);

	};
}


