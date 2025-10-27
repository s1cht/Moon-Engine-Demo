#include "AssetManager.h"
#include <Core/Algorithm.hpp>

#include "Renderer/Assets/Image.h"

namespace ME::Manager
{
	AssetManager::AssetManager()
	{
	}

	AssetManager::~AssetManager()
	{
	}

	bool AssetManager::LoadAsset(const ME::Core::String& path, bool centered)
	{
		ME::Utility::AssetFileFormats fileFormat;
		ME::Utility::AssetLoadResult assetLoadResult;

		fileFormat = GetExtension(path);
		assetLoadResult = ME::Utility::AssetLoader::Load(path, centered, fileFormat);

		if (assetLoadResult.Meshes.Size() > 0)
		{
			for (auto mesh = assetLoadResult.Meshes.Begin(); mesh != assetLoadResult.Meshes.End(); ++mesh)
				m_Meshes.EmplaceBack(std::move((*mesh)));
		}
		if (assetLoadResult.Images.Size() > 0)
		{
			for (auto image = assetLoadResult.Images.Begin(); image != assetLoadResult.Images.End(); ++image)
				m_Images.EmplaceBack(std::move((*image)));
		}


		return true;
	}

	ME::Utility::AssetFileFormats AssetManager::GetExtension(const ME::Core::String& path) const
	{
		ME::Core::String result = TEXT("");

		for (SIZE_T i = path.Size() - 1; i >= 0; --i)
		{
			if (path[i] == TEXT('.'))
			{
				for (SIZE_T u = i; u < path.Size(); u++)
					result += path[u];
				break;
			}
		}

		if (result == TEXT(".obj"))
			return ME::Utility::AssetFileFormats::OBJ;
		else if (result == TEXT(".tga"))
			return ME::Utility::AssetFileFormats::TRG;
		else
			return ME::Utility::AssetFileFormats::None;
	}

	ME::Core::Memory::Reference<ME::Assets::Mesh> AssetManager::GetMesh(const ME::Core::String& meshName)
	{
		for (const auto& mesh : m_Meshes)
		{
			if (mesh->GetGroupName() == meshName)
			{
				return mesh;
			}
		}
		return nullptr;
	}

	ME::Core::Memory::Reference<Assets::Image> AssetManager::GetImage(const ME::Core::String& imageName)
	{
		for (const auto& image : m_Images)
		{
			if (image->GetName() == imageName)
			{
				return image;
			}
		}
		return nullptr;
	}
}
