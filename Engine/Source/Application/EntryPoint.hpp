#pragma once
#include <Core.hpp>
#include <Core/Utility/Logging/Logger.hpp>
#include <Core/Utility/MemWatch/MemWatch.hpp>
#include <Core/Platform/Base/IO.hpp>
#include <Core/Time.hpp>

#include "Application/Application.hpp"

extern ME::Application* ME::CreateApplication();

#ifdef PLATFORM_WINDOWS
BOOL WINAPI TerminationHandler(DWORD signal)
{
	if (signal == CTRL_CLOSE_EVENT || signal == CTRL_C_EVENT || signal == CTRL_BREAK_EVENT)
	{
		ME::Application::RequestShutdown();

		ME::Core::IO::DirectoryStorage::Shutdown();
		//	Pawn::MemWatch::OnExit();
		ME::Core::Utility::Logger::Shutdown();
		ME::Core::Clock::Time::Shutdown();
		return TRUE;
	}
	return FALSE;
}

int main(int32 argc, const asciichar** argv)
{
 	system("chcp 65001 >nul");
	SetConsoleCtrlHandler(TerminationHandler, TRUE);

	ME::Core::Clock::Time::Init();
	ME::Core::Utility::Logger::Init();

	for (int32 i = 0; i < argc; i++)
	{
		if (i == 0)
		{
			ME::Core::String programPath = argv[0];
			
			for (auto it = programPath.end() - 1; (*it) != TEXT('\\'); --it)
				programPath.PopBack();

			ME::Core::IO::DirectoryStorage::StoreDirectory(ME::Core::IO::DirectoryStorage::Directory(TEXT("ProgramPath"), programPath));
		}
	}

	auto app = ME::CreateApplication();
	app->Run();
	delete app;

	ME::Core::IO::DirectoryStorage::Shutdown();
	ME::Core::Utility::Logger::Shutdown();
	ME::Core::Clock::Time::Shutdown();

	return 0;
}
#endif