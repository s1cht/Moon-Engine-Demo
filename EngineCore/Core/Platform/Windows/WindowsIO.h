#pragma once

#include "Core/Platform/Base/IO.h"

namespace Pawn::IO
{
	class CORE_API WindowsFile : public File
	{
	public:
		WindowsFile();
		WindowsFile(const uchar* path);
		WindowsFile(const uchar* path, FileReadMode mode);
		WindowsFile(const String& path);
		WindowsFile(const String& path, FileReadMode mode);
		WindowsFile(const WindowsFile&) = delete;

		~WindowsFile() override;

	public:

		bool Open(const uchar* path, FileReadMode mode) override;
		void Close() override;

		bool Read(String& output, StringReadMode mode) override;
		bool Write(const String& input) override;
		bool Append(const String& output) override;

		bool Lock() override;
		void Flush() override;

		bool Seek(SIZE_T position) override;
		SIZE_T Tell() override;

		bool Eof() const override;
		bool IsOpen()  const override;

		SIZE_T GetFileSize() const override;
		bool ReadBinary(void* data, SIZE_T size) override;

		String ReadAll() override;

		IOError GetFileLastError() const override { return m_LastError; }

		FileInfo GetFileInfo() const override { return m_FileInfo; }

	private:
		HANDLE m_File;
		FileInfo m_FileInfo;
		FileReadMode m_FileReadMode;
		
		IOError m_LastError;

	private:
		bool m_Opened;
		bool m_EOF;

	};

}