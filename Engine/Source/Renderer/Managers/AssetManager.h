#pragma once

#include "Renderer/Assets/Mesh.hpp"
#include "Utility/AssetLoader.h"

#include <Core.hpp>
#include <Core/Utils/Logging/Logger.hpp>
#include <Core/Memory/Memory.hpp>
#include <Core/Containers/Array.hpp>
#include <Core/Containers/String/String.hpp>

#include "Renderer/Base/Texture.h"

namespace ME::Manager
{
	class MEAPI AssetManager
	{
	public:
		AssetManager();
		AssetManager(const AssetManager&) = delete;
		~AssetManager();

	public:
		bool LoadAsset(const ME::Core::String& path, bool centered = false);
		ME::Core::Memory::Reference<Assets::Mesh> GetMesh(const ME::Core::String& meshName);
		ME::Core::Memory::Reference<Assets::Image> GetImage(const ME::Core::String& imageName);

	public:
		static AssetManager& Get()
		{
			static AssetManager instance;
			return instance;
		}
		inline static bool Load(const ME::Core::String& path, bool centered = false) { return Get().LoadAsset(path, centered); }

	private:
		ME::Utility::AssetFileFormats GetExtension(const ME::Core::String& path) const;

	private:
		ME::Core::Containers::Array<ME::Core::Memory::Reference<Assets::Mesh>> m_Meshes;
		ME::Core::Containers::Array<ME::Core::Memory::Reference<Assets::Image>> m_Images;
		//Pawn::Core::Containers::Array<Pawn::Core::Memory::Reference<Assets::Mesh>> m_Meshes;
		//Pawn::Core::Containers::Array<Pawn::Core::Memory::Reference<Assets::Mesh>> m_Meshes;

	};
}

