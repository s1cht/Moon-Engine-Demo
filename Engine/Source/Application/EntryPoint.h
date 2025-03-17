#pragma once


#include "Application/Application.h"
#include "Core/Utils/MemWatch/MemWatch.h"

#ifdef PLATFORM_WINDOWS
extern Pawn::Application* Pawn::CreateApplication();

int main(const ansichar** argc, int32 argv)
{

	Pawn::MemWatch::Get();
	Pawn::Logger::Init();
	Pawn::MemWatch::EnableMemWatch();

	for (int32 i = 0; i < argv; i++)
	{
		PE_INFO("CMD Arguments: {0}, {1}", i, argc[i]);
	}

	auto app = Pawn::CreateApplication();
	app->Run();
	delete app;

	Pawn::MemWatch::OnExit();
}

#endif