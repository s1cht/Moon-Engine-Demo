#include "WindowsIO.h"
#include "Core/Platform/Windows/WindowsIncludes.h"

#define PE_FILETIME_TO_TIMEPOINT(ft) ((static_cast<uint64>(ft.dwHighDateTime) << 32) + ft.dwLowDateTime - 116444736000000000LL) / 10000000LL;

namespace Pawn::IO
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

	WindowsFile::WindowsFile(const String& path)
		: m_FileInfo({}), m_File(nullptr), m_LastError(IOError::OK), m_Opened(false), m_EOF(false)
	{
		Open(path.GetString(), FileReadMode::Default);
	}

	WindowsFile::WindowsFile(const String& path, FileReadMode mode)
		: m_FileInfo({}), m_File(nullptr), m_LastError(IOError::OK), m_Opened(false), m_EOF(false)
	{
		Open(path.GetString(), mode);
	}

	WindowsFile::~WindowsFile()
	{
		delete[] m_File;
	}

	bool WindowsFile::Open(const uchar* path, FileReadMode mode)
	{
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

		m_File = CreateFileW(path, readFlags, FILE_SHARE_DELETE,
			NULL,
			canCreateNewFile ? OPEN_ALWAYS : OPEN_EXISTING,
			readFlags == GENERIC_READ ? FILE_ATTRIBUTE_READONLY : FILE_ATTRIBUTE_NORMAL,
			nullptr);

		if (!m_File)
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

		m_FileInfo.Size =  static_cast<size_t>(info->nFileSizeLow) | (static_cast<size_t>(info.nFileSizeHigh) << 32);
		m_FileInfo.CreationTime.Tick = PE_FILETIME_TO_TIMEPOINT(info.ftCreationTime);
		m_FileInfo.LastAccessTime.Tick = PE_FILETIME_TO_TIMEPOINT(info.ftLastAccessTime);
		m_FileInfo.LastWriteTime.Tick = PE_FILETIME_TO_TIMEPOINT(info.ftLastWriteTime);
		m_FileInfo.Attributes = (uint32)info.dwFileAttributes;

		m_Opened = true;

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
	}

	bool WindowsFile::Read(String& output)
	{

	}

	bool WindowsFile::Write(const String& input)
	{

	}

	bool WindowsFile::Append(const String& output)
	{

	}

	bool WindowsFile::Seek(SIZE_T position)
	{

	}

	SIZE_T WindowsFile::Tell()
	{

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

	Pawn::String WindowsFile::ReadAll()
	{
		
	}

	void WindowsFile::Flush()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	bool WindowsFile::ReadBinary(void* data, SIZE_T size)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	bool WindowsFile::Lock()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	///////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// IO Functions //////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////

	Memory::Reference<File> PCreateTempFile(const uchar* name)
	{

	}

	Memory::Reference<File> POpenFile(const uchar* path)
	{
		Memory::Reference<File> file;

		file = Memory::MakeReference<File>(path);

		if (!file->IsOpen())
		{
			PE_ERROR(TEXT("IO: File can't be opened! Error: {0}"), (uint32)file->GetLastError());
			return nullptr;
		}

		return file;
	}

	bool PFileExists(const uchar* path)
	{

	}

	bool PDeleteFile(const uchar* path)
	{

	}

	bool PRenameFile(const uchar* oldPath, const uchar* newPath)
	{

	}

	bool PCreateFile(const uchar* path)
	{

	}

	bool PCreateDirectory(const uchar* path)
	{

	}

}

#undef PE_FILETIME_TO_TIMEPOINT