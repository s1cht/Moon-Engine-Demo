#pragma once

#ifdef _WIN32

extern Pawn::Application* Pawn::CreateApplication();

int main()
{
	Pawn::Logger::Init();
	PE_CORE_INFO("Hi! This is a log from core!");

	auto app = Pawn::CreateApplication();
	app->Run();
	delete app;
}

#endif