#pragma once

#include "Application/Application.h"
#include "Core/Utils/MemWatch/MemWatch.h"
#include "Core/Containers/String.h"
#include "Core/Platform/Base/IO.h"

extern Pawn::Application* Pawn::CreateApplication();

#ifdef PLATFORM_WINDOWS

int wmain(int32 argc, const uchar** argv)
{
	Pawn::MemWatch::Get();
	Pawn::Logger::Init();
	Pawn::MemWatch::EnableMemWatch();

	for (int32 i = 0; i < argc; i++)
	{
		if (i == 0)
		{
			Pawn::String programPath = argv[0];
			
			for (auto it = programPath.end(); (*it) != TEXT('\\'); it--)
				*it = TEXT('\0');

			Pawn::IO::DirectoryStorage::StoreDirectory(Pawn::IO::DirectoryStorage::Directory(TEXT("ProgramPath"), programPath));
		}
	}

	auto app = Pawn::CreateApplication();
	app->Run();
	delete app;

	Pawn::IO::DirectoryStorage::Shutdown();
	Pawn::MemWatch::OnExit();
}

#endif