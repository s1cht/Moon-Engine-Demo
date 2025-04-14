#pragma once

#include <Core.h>
#include <Core/Utils/Logging/Logger.h>

#include "Mesh.h"
#include "Utility/AssetLoader.h"

import Pawn.Core.Memory;
import Pawn.Core.Container.Array;
import Pawn.Core.Container.String;
import Pawn.Core.Container.StringView;

namespace Pawn::Assets
{
	class PAWN_API AssetManager
	{
	public:
		AssetManager();
		AssetManager(const AssetManager&) = delete;
		~AssetManager();

	public:
		bool LoadAsset(const Pawn::Core::Containers::String& path, bool centered = false);
		Pawn::Core::Memory::Reference<Assets::Mesh> GetMesh(const Pawn::Core::Containers::String& meshName);

	public:
		static AssetManager& Get()
		{
			static AssetManager instance;
			return instance;
		}
		inline static bool Load(const Pawn::Core::Containers::String& path, bool centered = false) { return Get().LoadAsset(path, centered); }

	private:
		Pawn::Utility::AssetFileFormats GetExtension(const Pawn::Core::Containers::String& path) const;

	private:
		Pawn::Core::Containers::Array<Pawn::Core::Memory::Reference<Assets::Mesh>> m_Meshes;
		//Pawn::Core::Containers::Array<Pawn::Core::Memory::Reference<Assets::Mesh>> m_Meshes;
		//Pawn::Core::Containers::Array<Pawn::Core::Memory::Reference<Assets::Mesh>> m_Meshes;

	};
}

