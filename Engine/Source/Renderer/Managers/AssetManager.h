#pragma once
#include <Core.hpp>
#include <Core/Utility/Logging/Logger.hpp>
#include <Core/Memory/Memory.hpp>
#include <Core/Containers/String.hpp>

#include "Renderer/Base/Texture.hpp"
#include "Renderer/Assets/Mesh.hpp"
#include "Utility/AssetLoader.hpp"

namespace ME::Manager
{
	class MEAPI AssetManager
	{
	public:
		AssetManager() = default;
		AssetManager(const AssetManager&) = delete;
		~AssetManager() = default;

	public:
		bool LoadAsset(const ME::Core::String& path, bool centered = false);
		ME::Core::Memory::Reference<Assets::Mesh> GetMesh(const ME::Core::String& meshName);
		ME::Core::Memory::Reference<Assets::Image> GetImage(const ME::Core::String& imageName);
		Assets::AudioFile GetAudioFile(const ME::Core::String& audioName);

	public:
		static AssetManager& Get()
		{
			static AssetManager instance;
			return instance;
		}
		inline static bool Load(const ME::Core::String& path, bool centered = false) { return Get().LoadAsset(path, centered); }

	private:
		ME::Utility::AssetFileFormats GetExtension(const ME::Core::String& path) const;
		ME::Core::String GetFileName(const ME::Core::String& path) const;

	private:
		ME::Core::Array<ME::Core::Memory::Reference<Assets::Mesh>> m_Meshes;
		ME::Core::Array<ME::Core::Memory::Reference<Assets::Image>> m_Images;
		ME::Core::UnorderedMap<ME::Core::String, Assets::AudioFile> m_AudioFiles;
	};
}

