#pragma once

#ifdef _WIN32

extern Pawn::Application* Pawn::CreateApplication();

int main()
{
	Pawn::Logger::Init();

	auto app = Pawn::CreateApplication();
	app->Run();
	delete app;
}

#endif