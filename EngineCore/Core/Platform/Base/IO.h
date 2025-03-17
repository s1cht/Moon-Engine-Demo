#pragma once

#include "Core.h"
#include "Core/Containers/String.h"
#include "Core/Misc/Timepoint.h"

namespace Pawn::IO
{
	enum class FileReadMode
	{
		Default = 0,
		WriteAndRead, Read

	};

	enum class IOError
	{
		OK = 0,
		UnknownMode,
		Unknown
	};

	struct FileInfo
	{
		String Name;
		SIZE_T Size;
		Time::Timepoint CreationTime;
		Time::Timepoint  LastAccessTime;
		Time::Timepoint  LastWriteTime;
		uint32 Attributes;

	};

	class CORE_API File
	{
	public:
		virtual ~File() {};

		virtual bool Open(const uchar* path, FileReadMode mode) = 0;

		virtual void Close() = 0;

		virtual bool Read(String& output) = 0;

		virtual bool Write(const String& input) = 0;

		virtual bool Append(const String& output) = 0;

		virtual bool Seek(SIZE_T position) = 0;

		virtual SIZE_T Tell() = 0;

		virtual bool Eof() const = 0;

		virtual bool IsOpen() const  = 0;

		virtual SIZE_T GetFileSize() const = 0;

		virtual String ReadAll() = 0;

		virtual void Flush() = 0;

		virtual bool ReadBinary(void* data, SIZE_T size) = 0;

		virtual IOError GetFileLastError() const  = 0;

		virtual bool Lock() = 0;

		virtual FileInfo GetFileInfo() const = 0;

	};


	/* Creates a temporary file				*/
	/* Windows contains this file in %temp%	*/
	extern CORE_API Memory::Reference<File> PCreateTempFile(const uchar* name);

	extern CORE_API Memory::Reference<File> POpenFile(const uchar* path);

	extern CORE_API bool PFileExists(const uchar* path);

	extern CORE_API bool PDeleteFile(const uchar* path);

	extern CORE_API bool PRenameFile(const uchar* oldPath, const uchar* newPath);

	extern CORE_API bool PCreateFile(const uchar* path);

	extern CORE_API bool PCreateDirectory(const uchar* path);

}