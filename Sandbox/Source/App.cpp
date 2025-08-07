#include <MoonEngine.hpp>
#include "SandboxLayer.h"

static ME::ApplicationProperties AppProps;

class App : public ME::Application
{
public:
	App()
		: ME::Application(AppProps)
	{
		PushLayer(new SandboxLayer());
	};
	~App() {};

};

ME::Application* ME::CreateApplication()
{
	AppProps = {};

	AppProps.ApplicationName = TEXT("Sandbox");
	AppProps.VersionMajor = 0;
	AppProps.VersionMinor = 1;
	AppProps.VersionPatch = 0;

	return new App();
}