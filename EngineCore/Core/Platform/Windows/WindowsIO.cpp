module;

#include "Core/Utils/Logging/Logger.h"
#include "Core/Platform/Windows/WindowsIncludes.h"

module Pawn.Core.IO;

#define PE_FILETIME_TO_TIMEPOINT(ft) ((static_cast<uint64>(ft.dwHighDateTime) << 32) + ft.dwLowDateTime - 116444736000000000LL) / 10000000LL;

namespace Pawn::Core::IO
{
	WindowsFile::WindowsFile()
		: m_FileInfo({}), m_File(nullptr), m_LastError(IOError::OK), m_Opened(false), m_EOF(false)
	{
	}

	WindowsFile::WindowsFile(const uchar* path)
		: m_FileInfo({}), m_File(nullptr), m_LastError(IOError::OK), m_Opened(false), m_EOF(false)
	{
		Open(path, FileReadMode::Default);
	}

	WindowsFile::WindowsFile(const uchar* path, FileReadMode mode)
		: m_FileInfo({}), m_File(nullptr), m_LastError(IOError::OK), m_Opened(false), m_EOF(false)
	{
		Open(path, mode);
	}

	WindowsFile::WindowsFile(const Containers::String& path)
		: m_FileInfo({}), m_File(nullptr), m_LastError(IOError::OK), m_Opened(false), m_EOF(false)
	{
		Open(path.GetString(), FileReadMode::Default);
	}

	WindowsFile::WindowsFile(const Containers::String& path, FileReadMode mode)
		: m_FileInfo({}), m_File(nullptr), m_LastError(IOError::OK), m_Opened(false), m_EOF(false)
	{
		Open(path.GetString(), mode);
	}

	WindowsFile::~WindowsFile()
	{
		Close();
	}

	bool WindowsFile::Open(const uchar* path, FileReadMode mode)
	{
		if (m_Opened)
		{
			Close();
		}

		bool canCreateNewFile = false;
		uint32 readFlags = 0;
		BY_HANDLE_FILE_INFORMATION info = {};

		switch (mode)
		{
		case FileReadMode::Default:
			readFlags = GENERIC_READ;
			break;
		case FileReadMode::WriteAndRead:
			readFlags = GENERIC_READ | GENERIC_WRITE;
			canCreateNewFile = true;
			break;
		case FileReadMode::Read:
			readFlags = GENERIC_READ;
			break;
		default:
			m_LastError = IOError::UnknownMode;
			m_Opened = false;
			return false;
		}

		m_File = CreateFileW(path, readFlags, NULL,
			NULL,
			canCreateNewFile ? OPEN_ALWAYS : OPEN_EXISTING,
			readFlags == GENERIC_READ ? FILE_ATTRIBUTE_READONLY : FILE_ATTRIBUTE_NORMAL,
			nullptr);

		if (m_File == INVALID_HANDLE_VALUE)
		{
			DWORD error = GetLastError();
			PE_ERROR("{0}", error);
			return false;
		}

		if (!GetFileInformationByHandle(m_File, &info))
		{
			DWORD error = GetLastError();
			PE_ERROR("{0}", error);
			CloseHandle(m_File);
			m_File = INVALID_HANDLE_VALUE;
			m_Opened = false;
			return false;
		}

		m_FileInfo.Size =  static_cast<size_t>(info.nFileSizeLow) | (static_cast<size_t>(info.nFileSizeHigh) << 32);
		m_FileInfo.CreationTime.Tick = PE_FILETIME_TO_TIMEPOINT(info.ftCreationTime);
		m_FileInfo.LastAccessTime.Tick = PE_FILETIME_TO_TIMEPOINT(info.ftLastAccessTime);
		m_FileInfo.LastWriteTime.Tick = PE_FILETIME_TO_TIMEPOINT(info.ftLastWriteTime);
		m_FileInfo.Attributes = (uint32)info.dwFileAttributes;

		m_Opened = true;

		m_LastError = IOError::OK;
		return true;
	}

	void WindowsFile::Close()
	{
		if (!m_Opened)
		{
			PE_ERROR("Tried to closed closed file!");
			return;
		}

		if (!CloseHandle(m_File))
		{
			PE_ERROR(TEXT("File failed to close! Error: {0}"), GetLastError());
		}
		m_Opened = false;
		m_EOF = true;
	}

	bool WindowsFile::Read(Containers::String& output, StringReadMode mode)
	{
		uchar byte = TEXT(' ');
		DWORD bytesRead;
		
		output.Clear();
		output.Resize(128);

		if (!m_Opened)
		{
			PE_ERROR("Reading invalid file!");
			m_LastError = IOError::FileNotOpened;
			return false;
		}

		while (!m_EOF)
		{
			if (!ReadFile(m_File, &byte, 1, &bytesRead, nullptr) || bytesRead == 0)
			{
				DWORD error = GetLastError();
				PE_ERROR("Failed to read file! Win32 error: {0}", error);
				m_LastError = IOError::ReadFileFailed;
				m_EOF = true;
				break;
			}
			if (mode == StringReadMode::Line && byte == '\n')
				break;
			else if (mode == StringReadMode::Word && byte == '\0')
				break;

			output += byte;
		}

		return output.GetSize() > 0;
	}

	bool WindowsFile::Write(const Containers::String& input)
	{
		if (m_FileReadMode == FileReadMode::Read)
		{
			PE_ERROR(TEXT("Trying to write into read-only file!"));
			m_LastError = IOError::FileIsReadOnly;
			return false;
		}

		if (!m_Opened)
		{
			PE_ERROR("Writing in invalid file!");
			m_LastError = IOError::FileNotOpened;
			return false;
		}

		DWORD bytesWritten;

		if (!WriteFile(m_File, input.GetString(), (uint32)input.GetSize(), &bytesWritten, nullptr))
		{
			DWORD error = GetLastError();
			PE_ERROR("Failed to write in file! Win32 error: {0}", error);
			m_LastError = IOError::WriteFileFailed;
			return false;
		}

		m_LastError = IOError::OK;
		return true;
	}

	bool WindowsFile::Append(const Containers::String& output)
	{
		if (m_FileReadMode == FileReadMode::Read)
		{
			PE_ERROR(TEXT("Trying to write into read-only file!"));
			m_LastError = IOError::FileIsReadOnly;
			return false;
		}

		if (!m_Opened)
		{
			PE_ERROR("Writing in invalid file!");
			m_LastError = IOError::FileNotOpened;
			return false;
		}

		if (!Seek(GetFileSize()))
		{
			return false;
		}

		return Write(output);
	}

	bool WindowsFile::Seek(SIZE_T position)
	{
		if (!m_Opened)
		{
			PE_ERROR("Reading invalid file!");
			m_LastError = IOError::FileNotOpened;
			return false;
		}

		LARGE_INTEGER li;
		li.QuadPart = static_cast<LONGLONG>(position);
		if (!SetFilePointerEx(m_File, li, nullptr, FILE_BEGIN))
		{
			m_LastError = IOError::SeekFailed;
			PE_ERROR("Failed to seek file! Win32 error: {0}", GetLastError());
			return false;
		}

		m_EOF = (position >= GetFileSize());
		m_LastError = IOError::OK;
		return true;
	}

	bool WindowsFile::SeekToLine(SIZE_T line)
	{
		if (!m_Opened)
		{
			m_LastError = IOError::FileNotOpened;
			return false;
		}

		if (!Seek(0))
			return false;

		uint32 currentLine = 0;
		char buffer[1];
		DWORD bytesRead;
		bool found = false;

		while (currentLine < line)
		{
			if (!ReadFile(m_File, buffer, 1, &bytesRead, nullptr) || bytesRead == 0)
			{
				m_EOF = true;
				m_LastError = IOError::EndOfFile;
				return false;
			}

			if (buffer[0] == '\n')
				currentLine++;
		}

		if (currentLine == line)
			found = true;
		else
			m_LastError = IOError::EndOfFile;

		m_EOF = (Tell() >= GetFileSize());
		return found;
	}

	SIZE_T WindowsFile::Tell()
	{
		if (!m_Opened)
		{
			return 0;
		}

		LARGE_INTEGER li;
		li.QuadPart = 0;
		if (!SetFilePointerEx(m_File, li, &li, FILE_CURRENT))
		{
			PE_ERROR("Failed to get file position! Win32 error: {0}", GetLastError());
			return 0;
		}

		return static_cast<SIZE_T>(li.QuadPart);
	}

	bool WindowsFile::Eof() const
{
		return m_EOF;
	}

	bool WindowsFile::IsOpen() const
{
		return m_Opened;
	}

	SIZE_T WindowsFile::GetFileSize() const
{
		return m_FileInfo.Size;
	}

	Containers::String WindowsFile::ReadAll()
	{
		Containers::String result;
		SIZE_T size;
		if (!m_Opened)
		{
			PE_ERROR("Reading invalid file!");
			m_LastError = IOError::FileNotOpened;
			return result;
		}

		Seek(0);
		size = GetFileSize();
		result.Resize(size);

		uchar* buffer = new uchar[size];
		if (ReadBinary(buffer, size))
			for (SIZE_T i = 0; i < size; ++i)
				result += buffer[i];

		delete[] buffer;
		return result;
	}

	void WindowsFile::Flush()
	{
		if (!m_Opened)
		{
			return;
		}

		if (!FlushFileBuffers(m_File))
		{
			PE_ERROR("File flush failed! Win32 error: {0}", GetLastError());
			m_LastError = IOError::FlushFailed;
		}
	}

	bool WindowsFile::ReadBinary(void* data, SIZE_T& size)
	{
		if (!m_Opened)
		{
			PE_ERROR("Binary reading in invalid file!");
			m_LastError = IOError::FileNotOpened;
			return false;
		}

		DWORD bytesRead;

		if (!ReadFile(m_File, data, static_cast<DWORD>(size), &bytesRead, nullptr))
		{
			PE_ERROR("Failed to read binary data! Win32 error: {0}", GetLastError());
			m_LastError = IOError::ReadFileFailed;
			return false;
		}

		if (bytesRead != size)
		{
			PE_WARN("Partially read the file! Requested size: {0}, got from file: {1}", bytesRead, size);
			m_LastError = IOError::PartialRead;
			m_EOF = true;
			return true;
		}

		m_LastError = IOError::OK;
		return true;
	}

	bool WindowsFile::Lock()
	{
		if (!m_Opened)
		{
			PE_ERROR("Trying to lock invalid file!");
			m_LastError = IOError::FileNotOpened;
			return false;
		}

		if (!LockFile(m_File, 0, 0, m_FileInfo.Size & 0xFFFFFFFF, m_FileInfo.Size >> 32))
		{
			PE_ERROR("File lock failed! Win32 error: {0}", GetLastError());
			m_LastError = IOError::LockFailed;
			return false;
		}

		m_LastError = IOError::OK;
		return true;
	}

	///////////////////////////////////////////////////////////////////////////////////
	/////////////////////////// WIN32 IO Functions ////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////

#ifdef PLATFORM_WINDOWS

	Memory::Reference<File> PCreateTempFile(const uchar* name)
	{
		uchar tempPath[MAX_PATH];
		uchar tempFileName[MAX_PATH];

		if (!GetTempPathW(MAX_PATH, tempPath))
		{
			PE_ERROR(TEXT("IO: Failed to get %temp% directory! Win32 error: {0}"), GetLastError());
			return nullptr;
		}

		if (!GetTempFileNameW(tempPath, name, 0, tempFileName))
		{
			PE_ERROR(TEXT("IO: Failed to create Temporary file! Win32 error: {0}"), GetLastError());
			return nullptr;
		}

		Memory::Reference<File> file = Memory::MakeReference<WindowsFile>(reinterpret_cast<const uchar*>(tempFileName), FileReadMode::WriteAndRead);
		if (!file->IsOpen())
		{
			PE_ERROR(TEXT("IO: Failed to open temporary file! Win32 error: {0}"), (uint32)(file->GetFileLastError()));
			return nullptr;
		}

		return file;
	}

	Memory::Reference<File> POpenFile(const uchar* path)
	{
		Memory::Reference<File> file;

		file = Memory::MakeReference<WindowsFile>(path);

		if (!file->IsOpen())
		{
			PE_ERROR(TEXT("IO: File can't be opened! Error: {0}"), (uint32)file->GetFileLastError());
			return Memory::MakeReference<WindowsFile>();
		}

		return file;
	}

	Memory::Reference<File> POpenFile(const uchar* path, FileReadMode mode)
	{
		Memory::Reference<File> file;

		file = Memory::MakeReference<WindowsFile>(path, mode);

		if (!file->IsOpen())
		{
			PE_ERROR(TEXT("IO: File can't be opened! Error: {0}"), (uint32)file->GetFileLastError());
			return Memory::MakeReference<WindowsFile>();
		}

		return file;
	}

	bool PFileExists(const uchar* path)
	{
		DWORD attributes = GetFileAttributesW(path);
		if (attributes == INVALID_FILE_ATTRIBUTES)
		{
			DWORD error = GetLastError();
			if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND)
				return false;
			PE_ERROR(TEXT("IO: Failed to check file for exitsting! Win32 error: {0}"), error);
			return false;
		}
		return true;
	}

	bool PDeleteFile(const uchar* path)
	{
		if (!DeleteFileW(path))
		{
			DWORD error = GetLastError();
			if (error == ERROR_FILE_NOT_FOUND)
				return true;
			PE_ERROR(TEXT("IO: Failed to delete file! Win32 error: {0}"), error);
			return false;
		}
		return true;
	}

	bool PRenameFile(const uchar* oldPath, const uchar* newPath)
	{
		if (!MoveFileW(oldPath, newPath))
		{
			PE_ERROR(TEXT("IO: Failed to rename file: %u"), GetLastError());
			return false;
		}
		return true;
	}

	bool PCreateFile(const uchar* path)
	{
		HANDLE file = CreateFileW(
			path,
			GENERIC_WRITE,
			0,
			NULL,
			CREATE_NEW,
			FILE_ATTRIBUTE_NORMAL,
			nullptr
		);

		if (file == INVALID_HANDLE_VALUE)
		{
			PE_ERROR(TEXT("IO: Failed to create new file! Win32 error: {0}"), GetLastError());
			return false;
		}

		CloseHandle(file);
		return true;
	}

	bool PCreateDirectory(const uchar* path)
	{
		if (!CreateDirectoryW(path, NULL))
		{
			DWORD error = GetLastError();
			if (error == ERROR_ALREADY_EXISTS)
				return true;
			PE_ERROR(TEXT("IO: Failed to create directory! Win32 error: {0}"), error);
			return false;
		}
		return true;
	}

#endif
}

#undef PE_FILETIME_TO_TIMEPOINT