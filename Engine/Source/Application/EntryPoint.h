#pragma once

#include "pch.h"
#include "Application/Application.h"
#include "Core/Utils/MemWatch/MemWatch.h"

#ifdef PLATFORM_WINDOWS
extern Pawn::Application* Pawn::CreateApplication();

int main()
{
	Pawn::MemWatch::Get();
	Pawn::Logger::Init();
	Pawn::MemWatch::EnableMemWatch();

	auto app = Pawn::CreateApplication();
	app->Run();
	delete app;

	Pawn::MemWatch::OnExit();
}

#endif