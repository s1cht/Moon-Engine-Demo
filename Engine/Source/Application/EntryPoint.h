#pragma once

#include "pch.h"
#include "Application/Application.h"

#ifdef PLATFORM_WINDOWS
extern Pawn::Application* Pawn::CreateApplication();

int main()
{
	Pawn::Logger::Init();

	auto app = Pawn::CreateApplication();
	app->Run();
	delete app;
}

#endif