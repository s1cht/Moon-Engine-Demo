#include "AssetManager.h"
#include <Core/Containers/Algorithm.hpp>

namespace ME::Assets
{
	AssetManager::AssetManager()
	{
	}

	AssetManager::~AssetManager()
	{
	}

	bool AssetManager::LoadAsset(const ME::Core::Containers::String& path, bool centered)
	{
		ME::Utility::AssetFileFormats fileFormat;
		ME::Utility::AssetLoadResult assetLoadResult;

		fileFormat = GetExtension(path);
		assetLoadResult = ME::Utility::AssetLoader::Load(path, centered, fileFormat);

		if (assetLoadResult.Meshes.GetSize() == 0)
			return false;

		for (auto mesh = assetLoadResult.Meshes.Begin(); mesh != assetLoadResult.Meshes.End(); ++mesh)
		{
			(*mesh)->CreateBuffers();
			m_Meshes.EmplaceBack(std::move((*mesh)));
		}

		return true;
	}

	ME::Utility::AssetFileFormats AssetManager::GetExtension(const ME::Core::Containers::String& path) const
	{
		ME::Core::Containers::String result = TEXT("");

		for (SIZE_T i = path.GetSize() - 1; i >= 0; --i)
		{
			if (path[i] == TEXT('.'))
			{
				for (SIZE_T u = i; u < path.GetSize(); u++)
					result += path[u];
				break;
			}
		}

		if (result == TEXT(".obj"))
			return ME::Utility::AssetFileFormats::OBJ;
		else
			return ME::Utility::AssetFileFormats::None;
	}

	ME::Core::Memory::Reference<ME::Assets::Mesh> AssetManager::GetMesh(const ME::Core::Containers::String& meshName)
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

}
