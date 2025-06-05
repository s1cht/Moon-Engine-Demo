#include <PawnEngine.h>
#include "SandboxLayer.h"

static Pawn::ApplicationProperties AppProps;

class App : public Pawn::Application
{
public:
	App()
		: Pawn::Application(AppProps)
	{
		PushLayer(new SandboxLayer());
	};
	~App() {};

};

Pawn::Application* Pawn::CreateApplication()
{
	AppProps = {};

	AppProps.ApplicationName = TEXT("Sandbox");
	AppProps.VersionMajor = 1;
	AppProps.VersionMinor = 0;
	AppProps.VersionPatch = 0;

	return new App();
}