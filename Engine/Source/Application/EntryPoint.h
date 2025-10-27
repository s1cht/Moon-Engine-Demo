#pragma once

#include "Application/Application.h"
#include "Renderer/Base/Shader.h"
#include <Core/Utils/Logging/Logger.hpp>
#include <Core/Utils/MemWatch/MemWatch.hpp>

#include <Core/Platform/Base/IO.hpp>
#include <Core/Misc/Time.hpp>

extern ME::Application* ME::CreateApplication();

#ifdef PLATFORM_WINDOWS

BOOL WINAPI TerminationHandler(DWORD signal)
{
	if (signal == CTRL_CLOSE_EVENT || signal == CTRL_C_EVENT || signal == CTRL_BREAK_EVENT)
	{
		ME::Application::RequestShutdown();

		ME::Core::IO::DirectoryStorage::Shutdown();
		//	Pawn::MemWatch::OnExit();
		ME::Core::Utils::Logger::Shutdown();
		ME::Core::Clock::Time::Shutdown();
		return TRUE;
	}
	return FALSE;
}

int wmain(int32 argc, const char8** argv)
{
	SetConsoleCtrlHandler(TerminationHandler, TRUE);

	ME::Core::Clock::Time::Init();
//	Pawn::MemWatch::Get();
	ME::Core::Utils::Logger::Init();
//	Pawn::MemWatch::EnableMemWatch();

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
//	Pawn::MemWatch::OnExit();
	ME::Core::Utils::Logger::Shutdown();
	ME::Core::Clock::Time::Shutdown();

	return 0;
}

#endif