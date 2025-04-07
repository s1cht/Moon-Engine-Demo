#include "AssetManager.h"
#include <Core/Misc/Assertion.h>

import Pawn.Core.Algorithm;

namespace Pawn::Assets
{
	AssetManager::AssetManager()
	{
	}

	AssetManager::~AssetManager()
	{
	}

	bool AssetManager::LoadAsset(const Pawn::Core::Containers::String& path)
	{
		Pawn::Utility::AssetFileFormats fileFormat;
		Pawn::Utility::AssetLoadResult assetLoadResult;

		fileFormat = GetExtension(path);
		assetLoadResult = Pawn::Utility::AssetLoader::Load(path, fileFormat);

		if (assetLoadResult.Meshes.GetSize() == 0)
			return false;

		for (auto mesh = assetLoadResult.Meshes.End(); mesh != assetLoadResult.Meshes.Begin(); --mesh)
		{
			(*mesh)->CreateBuffers();
			m_Meshes.EmplaceBack(std::move((*mesh)));
		}

		return true;
	}

	Pawn::Utility::AssetFileFormats AssetManager::GetExtension(const Pawn::Core::Containers::String& path) const
	{
		Pawn::Core::Containers::String result = TEXT("");

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
			return Pawn::Utility::AssetFileFormats::OBJ;
		else
			return Pawn::Utility::AssetFileFormats::None;
	}

	Pawn::Core::Memory::Reference<Pawn::Assets::Mesh> AssetManager::GetMesh(const Pawn::Core::Containers::String& meshName)
	{
		for (auto mesh = m_Meshes.Begin(); mesh != m_Meshes.End(); ++mesh)
			if ((*mesh)->GetGroupName() == meshName)
				return (*mesh);
		return nullptr;
	}

}
