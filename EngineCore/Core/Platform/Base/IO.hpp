#pragma once

#include "Core.hpp"
#include "Core/Containers/Array.hpp"
#include "Core/Containers/String.hpp"
#include "Core/Time.hpp"

namespace ME::Core::IO
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
		ME::Core::String Name;
		SIZE_T Size;
		ME::Core::Clock::Timepoint CreationTime;
		ME::Core::Clock::Timepoint  LastAccessTime;
		ME::Core::Clock::Timepoint  LastWriteTime;
		uint32 Attributes;
	};

	class COREAPI File
	{
	public:
		virtual ~File() {}

		virtual bool Open(const char8* path, FileReadMode mode) = 0;
		virtual void Close() = 0;

		virtual bool Read(ME::Core::String& output, StringReadMode mode) = 0;
		virtual ME::Core::String ReadAll() = 0;
		virtual bool ReadBinary(void* data, const SIZE_T& size) = 0;

		virtual bool Write(const ME::Core::String& input) = 0;
		virtual bool RawWrite(void* input, SIZE_T size) = 0;
		virtual bool Append(const ME::Core::String& output) = 0;

		virtual SIZE_T Tell() = 0;

		virtual bool Seek(SIZE_T position) = 0;
		virtual bool SeekToLine(SIZE_T line) = 0;

		virtual bool Eof() const = 0;
		virtual bool IsOpen() const = 0;

		virtual void Flush() = 0;
		virtual bool Lock() = 0;

		virtual SIZE_T GetFileSize() const = 0;
		virtual IOError GetFileLastError() const = 0;
		virtual FileInfo GetFileInfo() const = 0;
	};

	/* Creates a temporary file				*/
	/* Windows contains this file in %temp%	*/
	extern COREAPI ME::Core::Memory::Reference<File> PCreateTempFile(const char8* name);
	extern COREAPI ME::Core::Memory::Reference<File> POpenFile(const char8* path);
	extern COREAPI ME::Core::Memory::Reference<File> POpenFile(const char8* path, FileReadMode mode);
	extern COREAPI bool PFileExists(const char8* path);
	extern COREAPI bool PDeleteFile(const char8* path);
	extern COREAPI bool PRenameFile(const char8* oldPath, const char8* newPath);
	extern COREAPI bool PCreateFile(const char8* path);
	extern COREAPI bool PCreateDirectory(const char8* path);

	class COREAPI DirectoryStorage
	{
	public:
		struct Directory
		{
			const ME::Core::String DirectoryName;
			const ME::Core::String DirectoryPath;
		};

	public:
		static bool StoreDirectory(Directory dir);
		static ME::Core::String GetDirectory(ME::Core::String directoryName);
		static void Shutdown();

	private:
		static ME::Core::Array<Directory*> s_Directories;

	};
}