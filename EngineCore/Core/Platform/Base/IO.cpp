#include "IO.h"

namespace Pawn::IO
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

	Pawn::String DirectoryStorage::GetDirectory(String directoryName)
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

	Pawn::Array<Pawn::IO::DirectoryStorage::Directory*, 30> DirectoryStorage::s_Directories;

}