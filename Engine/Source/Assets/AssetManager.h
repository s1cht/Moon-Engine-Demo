#pragma once

#include <Core.hpp>
#include <Core/Utils/Logging/Logger.hpp>

#include "Mesh.h"
#include "Utility/AssetLoader.h"

#include <Core/Memory/Memory.hpp>
#include <Core/Containers/Array.hpp>
#include <Core/Containers/String/String.hpp>

#include "Renderer/Base/Texture.h"

namespace ME::Assets
{
	class MOON_API AssetManager
	{
	public:
		AssetManager();
		AssetManager(const AssetManager&) = delete;
		~AssetManager();

	public:
		bool LoadAsset(const ME::Core::Containers::String& path, bool centered = false);
		ME::Core::Memory::Reference<Assets::Mesh> GetMesh(const ME::Core::Containers::String& meshName);
		ME::Core::Memory::Reference<Assets::Image> GetImage(const ME::Core::Containers::String& imageName);

	public:
		static AssetManager& Get()
		{
			static AssetManager instance;
			return instance;
		}
		inline static bool Load(const ME::Core::Containers::String& path, bool centered = false) { return Get().LoadAsset(path, centered); }

	private:
		ME::Utility::AssetFileFormats GetExtension(const ME::Core::Containers::String& path) const;

	private:
		ME::Core::Containers::Array<ME::Core::Memory::Reference<Assets::Mesh>> m_Meshes;
		ME::Core::Containers::Array<ME::Core::Memory::Reference<Assets::Image>> m_Images;
		//Pawn::Core::Containers::Array<Pawn::Core::Memory::Reference<Assets::Mesh>> m_Meshes;
		//Pawn::Core::Containers::Array<Pawn::Core::Memory::Reference<Assets::Mesh>> m_Meshes;

	};
}

