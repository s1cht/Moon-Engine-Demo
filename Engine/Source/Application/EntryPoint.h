#pragma once

#include "Application/Application.h"
#include "Renderer/Base/Shader.h"
#include <Core/Utils/Logging/Logger.hpp>
#include <Core/Utils/MemWatch/MemWatch.hpp>

#include <Core/Platform/Base/IO.hpp>
#include <Core/Misc/Time.hpp>

extern Pawn::Application* Pawn::CreateApplication();

#ifdef PLATFORM_WINDOWS

int wmain(int32 argc, const uchar** argv)
{
	Pawn::Core::Clock::Time::Init();
	Pawn::MemWatch::Get();
	Pawn::Core::Utils::Logger::Init();
	Pawn::MemWatch::EnableMemWatch();

	for (int32 i = 0; i < argc; i++)
	{
		if (i == 0)
		{
			Pawn::Core::Containers::String programPath = argv[0];
			
			for (auto it = programPath.end() - 1; (*it) != TEXT('\\'); --it)
				programPath.PopBack();

			Pawn::Core::IO::DirectoryStorage::StoreDirectory(Pawn::Core::IO::DirectoryStorage::Directory(TEXT("ProgramPath"), programPath));
		}
	}

	Pawn::Render::Shader::SetShaderSourceExtension(TEXT(".pshader"));
	Pawn::Render::Shader::SetCompiledShaderExtension(TEXT(".cpshader"));

	auto app = Pawn::CreateApplication();
	app->Run();
	delete app;


	Pawn::Core::IO::DirectoryStorage::Shutdown();
	Pawn::MemWatch::OnExit();
	Pawn::Core::Utils::Logger::Shutdown();
	Pawn::Core::Clock::Time::Shutdown();
}

#endif