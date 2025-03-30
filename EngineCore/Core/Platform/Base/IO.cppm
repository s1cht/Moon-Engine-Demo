module;

#include "Core.h"

export module Pawn.Core.IO.Base;

import Pawn.Core.Memory;
import Pawn.Core.Clock;
import Pawn.Core.Container.Array;
import Pawn.Core.Container.String;
import Pawn.Core.Container.UnorderedMap;

#define PE_FAILED_IO(result) (result != Pawn::IO::IOError::OK);
#define PE_FAILED_IO_INT(result) (result != (int32)Pawn::IO::IOError::OK);

export namespace Pawn::Core::IO
{
	enum class FileReadMode
	{
		Default = 0,
		WriteAndRead, Read

	};

	enum class StringReadMode
	{
		Line,
		Word,
	};

	enum class IOError
	{
		OK = 0,
		FileNotOpened,
		FileIsReadOnly,
		ReadFileFailed,
		WriteFileFailed,
		SeekFailed,
		PartialRead,
		FlushFailed,
		LockFailed,
		EndOfFile,
		UnknownMode,
		Unknown
	};

	struct FileInfo
	{
		Pawn::Core::Containers::String Name;
		SIZE_T Size;
		Pawn::Core::Clock::Timepoint CreationTime;
		Pawn::Core::Clock::Timepoint  LastAccessTime;
		Pawn::Core::Clock::Timepoint  LastWriteTime;
		uint32 Attributes;

	};

	class CORE_API File
	{
	public:
		virtual ~File() {};

		virtual bool Open(const uchar* path, FileReadMode mode) = 0;

		virtual void Close() = 0;

		virtual bool Read(Pawn::Core::Containers::String& output, StringReadMode mode) = 0;

		virtual bool Write(const Pawn::Core::Containers::String& input) = 0;

		virtual bool Append(const Pawn::Core::Containers::String& output) = 0;

		virtual bool Seek(SIZE_T position) = 0;

		virtual bool SeekToLine(SIZE_T line) = 0;

		virtual SIZE_T Tell() = 0;

		virtual bool Eof() const = 0;

		virtual bool IsOpen() const  = 0;

		virtual SIZE_T GetFileSize() const = 0;

		virtual Pawn::Core::Containers::String ReadAll() = 0;

		virtual void Flush() = 0;

		virtual bool ReadBinary(void* data, SIZE_T& size) = 0;

		virtual IOError GetFileLastError() const  = 0;

		virtual bool Lock() = 0;

		virtual FileInfo GetFileInfo() const = 0;

	};

	/* Creates a temporary file				*/
	/* Windows contains this file in %temp%	*/
	extern CORE_API Pawn::Core::Memory::Reference<File> PCreateTempFile(const uchar* name);

	extern CORE_API Pawn::Core::Memory::Reference<File> POpenFile(const uchar* path);

	extern CORE_API Pawn::Core::Memory::Reference<File> POpenFile(const uchar* path, FileReadMode mode);

	extern CORE_API bool PFileExists(const uchar* path);

	extern CORE_API bool PDeleteFile(const uchar* path);

	extern CORE_API bool PRenameFile(const uchar* oldPath, const uchar* newPath);

	extern CORE_API bool PCreateFile(const uchar* path);

	extern CORE_API bool PCreateDirectory(const uchar* path);
	


	class CORE_API DirectoryStorage
	{
	public:
		struct Directory
		{
			const Pawn::Core::Containers::String DirectoryName;
			const Pawn::Core::Containers::String DirectoryPath;
		};

	public:
		static bool StoreDirectory(Directory dir);
		static Pawn::Core::Containers::String GetDirectory(Pawn::Core::Containers::String directoryName);
		static void Shutdown();

	private:
		static  Pawn::Core::Containers::Array<Directory*, 30> s_Directories;

	};

}