#pragma once

#include <Core.hpp>
#include <Core/Utils/Logging/Logger.hpp>

#include "Mesh.h"
#include "Utility/AssetLoader.h"

#include <Core/Memory/Memory.hpp>
#include <Core/Containers/Array.hpp>
#include <Core/Containers/String/String.hpp>

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

