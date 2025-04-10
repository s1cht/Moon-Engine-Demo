module;

#include "Core.h"

module Pawn.Core.IO.Base;

namespace Pawn::Core::IO
{
	bool DirectoryStorage::StoreDirectory(Directory dir)
	{
		bool result;
		result = PFileExists(dir.DirectoryPath.GetString());

		if (!result)
			return false;

		s_Directories.PushBack(new Directory(dir));
		return true;
	}

	Containers::String DirectoryStorage::GetDirectory(Containers::String directoryName)
	{
		for (Directory* dir : s_Directories)
		{
			if (dir->DirectoryName == directoryName)
			{
				return dir->DirectoryPath;
				break;
			}
		}

		return TEXT("");
	}

	void DirectoryStorage::Shutdown()
	{
		for (Directory* dir : s_Directories)
		{
			delete dir;
		}
	}

	Containers::Array<DirectoryStorage::Directory*> DirectoryStorage::s_Directories;

}