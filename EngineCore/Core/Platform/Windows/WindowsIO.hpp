#pragma once

#include "Core.hpp"
#include "Core/Platform/Base/IO.hpp"
#include "Core/Containers/String.hpp"

namespace ME::Core::IO
{
	class COREAPI WindowsFile final : public File
	{
	public:
		WindowsFile();
		WindowsFile(const char8* path);
		WindowsFile(const char8* path, FileReadMode mode);
		WindowsFile(const String& path);
		WindowsFile(const String& path, FileReadMode mode);
		WindowsFile(const WindowsFile&) = delete;

		~WindowsFile() override;

	public:
		bool Open(const char8* path, FileReadMode mode) override;
		void Close() override;

		bool Read(String& output, StringReadMode mode) override;
		String ReadAll() override;
		bool ReadBinary(void* data, SIZE_T& size) override;

	    bool Write(const String& input) override;
		bool RawWrite(void* input, SIZE_T size) override;
		bool Append(const String& output) override;

		SIZE_T Tell() override;

	    bool Seek(SIZE_T position) override;
		bool SeekToLine(SIZE_T line) override;

		bool Eof() const override;
		bool IsOpen()  const override;

		void Flush() override;
	    bool Lock() override;

		SIZE_T GetFileSize() const override;
		IOError GetFileLastError() const override { return m_LastError; }
		FileInfo GetFileInfo() const override { return m_FileInfo; }

	private:
		ME::Core::WideString GetFileName();

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