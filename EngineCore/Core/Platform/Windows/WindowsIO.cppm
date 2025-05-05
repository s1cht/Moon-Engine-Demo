module;
export module Pawn.Core.IO;

#include "Core.h"
import Pawn.Core.Types;
import Pawn.Core.Container.String;

export import Pawn.Core.IO.Base;

export namespace Pawn::Core::IO
{
	class CORE_API WindowsFile : public File
	{
	public:
		WindowsFile();
		WindowsFile(const uchar* path);
		WindowsFile(const uchar* path, FileReadMode mode);
		WindowsFile(const Containers::String& path);
		WindowsFile(const Containers::String& path, FileReadMode mode);
		WindowsFile(const WindowsFile&) = delete;

		~WindowsFile() override;

	public:

		bool Open(const uchar* path, FileReadMode mode) override;
		void Close() override;

		bool Read(Containers::String& output, StringReadMode mode) override;
		bool Write(const Containers::String& input) override;
		bool RawWrite(void* input, SIZE_T size) override;
		bool Append(const Containers::String& output) override;

		bool Lock() override;
		void Flush() override;

		bool Seek(SIZE_T position) override;
		bool SeekToLine(SIZE_T line) override;
		SIZE_T Tell() override;

		bool Eof() const override;
		bool IsOpen()  const override;

		SIZE_T GetFileSize() const override;
		bool ReadBinary(void* data, SIZE_T& size) override;

		Containers::String ReadAll() override;

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