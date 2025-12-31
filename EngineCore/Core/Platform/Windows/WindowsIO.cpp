#include "WindowsIO.hpp"
#include <psapi.h>
#include <Shlwapi.h>
#include <tchar.h>
#include <strsafe.h>

#include "Core.hpp"

#define ME_FILETIME_TO_TIMEPOINT(ft) ((static_cast<uint64>((ft).dwHighDateTime) << 32) + (ft).dwLowDateTime - 116444736000000000LL) / 10000000LL

namespace ME::Core::IO
{
	WindowsFile::WindowsFile()
		: m_FileInfo({}), m_File(nullptr), m_LastError(IOError::OK), m_Opened(false), m_EOF(false)
	{
	}

	WindowsFile::WindowsFile(const char8* path)
		: m_FileInfo({}), m_File(nullptr), m_LastError(IOError::OK), m_Opened(false), m_EOF(false)
	{
		Open(path, FileReadMode::Default);
	}

	WindowsFile::WindowsFile(const char8* path, FileReadMode mode)
		: m_FileInfo({}), m_File(nullptr), m_LastError(IOError::OK), m_Opened(false), m_EOF(false)
	{
		Open(path, mode);
	}

	WindowsFile::WindowsFile(const String& path)
		: m_FileInfo({}), m_File(nullptr), m_LastError(IOError::OK), m_Opened(false), m_EOF(false)
	{
		Open(path.String(), FileReadMode::Default);
	}

	WindowsFile::WindowsFile(const String& path, FileReadMode mode)
		: m_FileInfo({}), m_File(nullptr), m_LastError(IOError::OK), m_Opened(false), m_EOF(false)
	{
		Open(path.String(), mode);
	}

	WindowsFile::~WindowsFile()
	{
		Close();
	}

	bool WindowsFile::Open(const char8* path, FileReadMode mode)
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

		m_FileReadMode = mode;

		m_File = CreateFileW(Core::StringToWideString(path).String(), readFlags, NULL,
			NULL,
			canCreateNewFile ? OPEN_ALWAYS : OPEN_EXISTING,
			readFlags == GENERIC_READ ? FILE_ATTRIBUTE_READONLY : FILE_ATTRIBUTE_NORMAL,
			nullptr);

		if (m_File == INVALID_HANDLE_VALUE)
		{
			DWORD error = GetLastError();
			ME_ERROR("WindowsIO: file wasn't created! Win32 error: {0}", error);
			return false;
		}

		if (!GetFileInformationByHandle(m_File, &info))
		{
			DWORD error = GetLastError();
			ME_ERROR("WindowsIO: couldn't get file information! Win32 error: {0}", error);
			CloseHandle(m_File);
			m_File = INVALID_HANDLE_VALUE;
			m_Opened = false;
			return false;
		}

		Core::WideString wpath = StringToWideString(path);
		SIZE_T len = wcslen(wpath.String()) + 1;
		wchar* name = new wchar[len];
		wcscpy_s(name, len, wpath.String());
		PathStripPathW(name);
		m_FileInfo.Name = WideStringToString(name);
		m_FileInfo.Size = static_cast<SIZE_T>(info.nFileSizeLow) | (static_cast<SIZE_T>(info.nFileSizeHigh) << 32);
		m_FileInfo.CreationTime.Tick = ME_FILETIME_TO_TIMEPOINT(info.ftCreationTime);
		m_FileInfo.LastAccessTime.Tick = ME_FILETIME_TO_TIMEPOINT(info.ftLastAccessTime);
		m_FileInfo.LastWriteTime.Tick = ME_FILETIME_TO_TIMEPOINT(info.ftLastWriteTime);
		m_FileInfo.Attributes = static_cast<uint32>(info.dwFileAttributes);

		delete[] name;

		m_Opened = true;

		m_LastError = IOError::OK;
		return true;
	}

	void WindowsFile::Close()
	{
		if (!m_Opened)
		{
			ME_ERROR("Tried to closed closed file!");
			return;
		}

		Flush();

		if (!CloseHandle(m_File))
		{
			ME_ERROR("File failed to close! Error: {0}", GetLastError());
		}
		m_Opened = false;
		m_EOF = true;
	}

	bool WindowsFile::Read(String& output, StringReadMode mode)
	{
		char8 byte = 0;
		DWORD bytesRead;
		bool readingStarted = false;

		output.Clear();

		if (!m_Opened)
		{
			ME_ERROR("Reading invalid file!");
			m_LastError = IOError::FileNotOpened;
			return false;
		}

		while (!m_EOF)
		{
			if (Tell() >= GetFileSize())
			{
				m_EOF = true;
				break;
			}

			if (!ReadFile(m_File, &byte, 1, &bytesRead, nullptr))
			{
				DWORD error = GetLastError();
				ME_ERROR("Failed to read file! Win32 error: {0}, {1}", error, Tell());
				m_LastError = IOError::ReadFileFailed;
				m_EOF = true;
				break;
			}
			if (bytesRead == 0)
			{
				m_EOF = true;
				break;
			}
			if (mode == StringReadMode::Line && byte == '\n')
				break;
			if (mode == StringReadMode::Word)
			{
				if (byte == '\n' || byte == '\r' || byte == '\0' || byte == 32)
				{
					if (readingStarted)
						break;
					else
						continue;
				}
			    readingStarted = true;
			}

		    output += byte;
		}
	
		return !output.Empty();
	}

	bool WindowsFile::RawWrite(void* input, SIZE_T size)
	{
		if (m_FileReadMode == FileReadMode::Read)
		{
			ME_ERROR("Trying to write into read-only file!");
			m_LastError = IOError::FileIsReadOnly;
			return false;
		}

		if (!m_Opened)
		{
			ME_ERROR("Writing in invalid file!");
			m_LastError = IOError::FileNotOpened;
			return false;
		}

		DWORD bytesWritten;

		if (!WriteFile(m_File, input, static_cast<uint32>(size), &bytesWritten, nullptr))
		{
			DWORD error = GetLastError();
			ME_ERROR("Failed to write in file! Win32 error: {0}", error);
			m_LastError = IOError::WriteFileFailed;
			return false;
		}

		m_LastError = IOError::OK;
		return true;
	}

	bool WindowsFile::Write(const String& input)
	{
		if (m_FileReadMode == FileReadMode::Read)
		{
			ME_ERROR("Trying to write into read-only file!");
			m_LastError = IOError::FileIsReadOnly;
			return false;
		}

		if (!m_Opened)
		{
			ME_ERROR("Writing in invalid file!");
			m_LastError = IOError::FileNotOpened;
			return false;
		}

		DWORD bytesWritten;

		if (!WriteFile(m_File, input.String(), static_cast<uint32>(input.Size()), &bytesWritten, nullptr))
		{
			DWORD error = GetLastError();
			ME_ERROR("Failed to write in file! Win32 error: {0}", error);
			m_LastError = IOError::WriteFileFailed;
			return false;
		}

		m_LastError = IOError::OK;
		return true;
	}

	bool WindowsFile::Append(const String& output)
	{
		if (m_FileReadMode == FileReadMode::Read)
		{
			ME_ERROR("Trying to write into read-only file!");
			m_LastError = IOError::FileIsReadOnly;
			return false;
		}

		if (!m_Opened)
		{
			ME_ERROR("Writing in invalid file!");
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
			ME_ERROR("Reading invalid file!");
			m_LastError = IOError::FileNotOpened;
			return false;
		}

		LARGE_INTEGER li;
		li.QuadPart = static_cast<LONGLONG>(position);
		if (!SetFilePointerEx(m_File, li, nullptr, FILE_BEGIN))
		{
			m_LastError = IOError::SeekFailed;
			ME_ERROR("Failed to seek file! Win32 error: {0}", GetLastError());
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
			if (!ReadFile(m_File, buffer, 1, &bytesRead, nullptr))
			{
				m_EOF = true;
				m_LastError = IOError::EndOfFile;
				return false;
			}
			if (bytesRead == 0)
			{
				m_EOF = true;
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
			ME_ERROR("Failed to get file position! Win32 error: {0}", GetLastError());
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

	String WindowsFile::ReadAll()
	{
		String result;
        if (!m_Opened)
		{
			ME_ERROR("Reading invalid file!");
			m_LastError = IOError::FileNotOpened;
			return result;
		}

		Seek(0);
		SIZE_T size = GetFileSize();
		result.Resize(size);

		char8* buffer = new char8[size];
		if (ReadBinary(buffer, size))
			for (SIZE_T i = 0; i < size; ++i)
				result += buffer[i];

		delete[] buffer;
		return result;
	}

	ME::Core::WideString WindowsFile::GetFileName()
	{
		HANDLE hFileMap = CreateFileMapping(m_File,
			nullptr,
			PAGE_READONLY,
			0,
			1,
			nullptr);

		if (!hFileMap)
		{
			return TEXTW("");
		}

		wchar pszFilename[MAX_PATH + 1];

		return pszFilename;
	}

	void WindowsFile::Flush()
	{
		if (!m_Opened)
		{
			return;
		}

		if (!FlushFileBuffers(m_File))
		{
			ME_ERROR("File flush failed! Win32 error: {0}", GetLastError());
			m_LastError = IOError::FlushFailed;
		}
	}

	bool WindowsFile::ReadBinary(void* data, const SIZE_T& size)
	{
		if (!m_Opened)
		{
			ME_ERROR("Binary reading in invalid file!");
			m_LastError = IOError::FileNotOpened;
			return false;
		}

		DWORD bytesRead;

		if (!ReadFile(m_File, data, static_cast<DWORD>(size), &bytesRead, nullptr))
		{
			ME_ERROR("Failed to read binary data! Win32 error: {0}", GetLastError());
			m_LastError = IOError::ReadFileFailed;
			return false;
		}

		if (bytesRead != size)
		{
			ME_WARN("Partially read the file! Requested size: {0}, got from file: {1}", size, bytesRead);
			m_LastError = IOError::PartialRead;
			m_EOF = true;
			return false;
		}

		m_LastError = IOError::OK;
		return true;
	}

	bool WindowsFile::Lock()
	{
		if (!m_Opened)
		{
			ME_ERROR("Trying to lock invalid file!");
			m_LastError = IOError::FileNotOpened;
			return false;
		}

		if (!LockFile(m_File, 0, 0, m_FileInfo.Size & 0xFFFFFFFF, m_FileInfo.Size >> 32))
		{
			ME_ERROR("File lock failed! Win32 error: {0}", GetLastError());
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

	Memory::Reference<File> PCreateTempFile(const char8* name)
	{
		wchar tempPath[MAX_PATH];
		wchar tempFileName[MAX_PATH];

		if (!GetTempPathW(MAX_PATH, tempPath))
		{
			ME_ERROR("IO: Failed to get %temp% directory! Win32 error: {0}", GetLastError());
			return nullptr;
		}

		if (!GetTempFileNameW(tempPath, StringToWideString(name).String(), 0, tempFileName))
		{
			ME_ERROR("IO: Failed to create Temporary file! Win32 error: {0}", GetLastError());
			return nullptr;
		}

		Memory::Reference<File> file = Memory::MakeReference<WindowsFile>(reinterpret_cast<const char8*>(tempFileName), FileReadMode::WriteAndRead);
		if (!file->IsOpen())
		{
			ME_ERROR("IO: Failed to open temporary file! Win32 error: {0}", static_cast<uint32>(file->GetFileLastError()));
			return nullptr;
		}

		return file;
	}

	Memory::Reference<File> POpenFile(const char8* path)
	{
        Memory::Reference<File> file = Memory::MakeReference<WindowsFile>(path);
		if (!file->IsOpen())
		{
			ME_ERROR("IO: File can't be opened! Error: {0}", static_cast<uint32>(file->GetFileLastError()));
			return file;
		}
		return file;
	}

	Memory::Reference<File> POpenFile(const char8* path, FileReadMode mode)
	{
        Memory::Reference<File> file = Memory::MakeReference<WindowsFile>(path, mode);

		if (!file->IsOpen())
		{
			ME_ERROR("IO: File can't be opened! Error: {0}", static_cast<uint32>(file->GetFileLastError()));
			return Memory::MakeReference<WindowsFile>();
		}
		return file;
	}

	bool PFileExists(const char8* path)
	{
		DWORD attributes = GetFileAttributesW(StringToWideString(path).String());
		if (attributes == INVALID_FILE_ATTRIBUTES)
		{
			DWORD error = GetLastError();
			if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND)
				return false;
			ME_ERROR("IO: Failed to check file for existing! Win32 error: {0}", static_cast<uint32>(error));
			return false;
		}
		return true;
	}

	bool PDeleteFile(const char8* path)
	{
		if (!DeleteFileW(StringToWideString(path).String()))
		{
			DWORD error = GetLastError();
			if (error == ERROR_FILE_NOT_FOUND)
				return true;
			ME_ERROR("IO: Failed to delete file! Win32 error: {0}", static_cast<uint32>(error));
			return false;
		}
		return true;
	}

	bool PRenameFile(const char8* oldPath, const char8* newPath)
	{
		if (!MoveFileW(StringToWideString(oldPath).String(), StringToWideString(newPath).String()))
		{
			ME_ERROR("IO: Failed to rename file: {0}", GetLastError());
			return false;
		}
		return true;
	}

	bool PCreateFile(const char8* path)
	{
		HANDLE file = CreateFileW(
			StringToWideString(path).String(),
			GENERIC_WRITE,
			0,
			nullptr,
			CREATE_NEW,
			FILE_ATTRIBUTE_NORMAL,
			nullptr
		);

		if (file == INVALID_HANDLE_VALUE)
		{
			ME_ERROR("IO: Failed to create new file! Win32 error: {0}", GetLastError());
			return false;
		}

		CloseHandle(file);
		return true;
	}

	bool PCreateDirectory(const char8* path)
	{
		if (!CreateDirectoryW(StringToWideString(path).String(), nullptr))
		{
			DWORD error = GetLastError();
			if (error == ERROR_ALREADY_EXISTS)
				return true;
			ME_ERROR("IO: Failed to create directory! Win32 error: {0}", static_cast<uint32>(error));
			return false;
		}
		return true;
	}

#endif
}

#undef ME_FILETIME_TO_TIMEPOINT