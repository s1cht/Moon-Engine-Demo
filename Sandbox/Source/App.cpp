#include <MoonEngine.hpp>
#include "SandboxLayer.hpp"

static ME::ApplicationProperties AppProps;

class App : public ME::Application
{
public:
	App() : ME::Application(AppProps)
	{
		PushLayer(new SandboxLayer());
	}
	~App() = default;
};

ME::Application* ME::CreateApplication()
{
	AppProps = {};
	AppProps.ApplicationName = TEXT("Game");
	AppProps.VersionMajor = 0;
	AppProps.VersionMinor = 1;
	AppProps.VersionPatch = 0;

	AppProps.AssetPaths = {
		// Torch mesh
		Core::IO::DirectoryStorage::GetDirectory(TEXT("ProgramPath")) + Core::String(TEXT("assets/Meshes/torch.obj")),
		// Flashlight mesh
	    Core::IO::DirectoryStorage::GetDirectory(TEXT("ProgramPath")) + Core::String(TEXT("assets/Meshes/flashlight.obj")),
		// Stone texture
	    Core::IO::DirectoryStorage::GetDirectory(TEXT("ProgramPath")) + Core::String(TEXT("assets/Images/stone01.tga"))
    };

	AppProps.ShaderPaths = {};

	return new App();
}