#include "IO.hpp"

namespace ME::Core::IO
{
	bool DirectoryStorage::StoreDirectory(Directory dir)
	{
		bool result;
		result = PFileExists(dir.DirectoryPath.String());

		if (!result)
			return false;

		s_Directories.PushBack(new Directory(dir));
		return true;
	}

	String DirectoryStorage::GetDirectory(String directoryName)
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

    Array<DirectoryStorage::Directory*> DirectoryStorage::s_Directories;

}