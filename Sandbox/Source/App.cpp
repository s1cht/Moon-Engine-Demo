#include <PawnEngine.h>
#include "SandboxLayer.h"

class App : public Pawn::Application
{
public:
	App()
	{
		PushLayer(new SandboxLayer());
	};
	~App() {};

};

Pawn::Application* Pawn::CreateApplication()
{
	return new App();
}