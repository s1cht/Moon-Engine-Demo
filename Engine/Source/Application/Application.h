#pragma once

#include <Core.hpp>
#include <Core/Layer/LayerStack.hpp>
#include <Core/Events/Event.hpp>
#include <Core/Memory/Memory.hpp>

#include "Input/Input.h"
#include "Platform/Platform.h"
#include "Renderer/ImGui/ImGuiLayer.h"
#include "Events/WindowEvents.h"

namespace ME 
{
	struct MEAPI ApplicationProperties
	{
		int32 VersionMajor; 
		int32 VersionMinor;
		int32 VersionPatch;
		ME::Core::String ApplicationName;

		ME::Core::Containers::Array<ME::Core::String> AssetPaths;
		ME::Core::Containers::Array<ME::Render::Manager::ShaderGroupSpecification> ShaderPaths;

		ApplicationProperties(int32 verMajor = 1, int32 verMinor = 0, int32 verPatch = 0, ME::Core::String appName = TEXT("Application"))
			: VersionMajor(verMajor), VersionMinor(verMinor), VersionPatch(verPatch), ApplicationName(appName) {}
	};

	typedef ApplicationProperties ApplicationData;

	class MEAPI Application
	{
	public:
		Application(ApplicationProperties props = ApplicationProperties());
		virtual ~Application();

	public:
		Window* GetWindow() { return m_Window.get(); }

	public:
		void Run();

		void PushLayer(Core::Layer* layer);
		void PushOverlay(Core::Layer* overlay);

	public:
		const ApplicationData& GetAppData() const { return m_AppData; }

	public:
		void OnEvent(Core::Event& event);

	public:
		static Application& Get() { return *s_Instance; }
		static void RequestShutdown() { s_ShutdownRequested = true; }

	private:
		bool m_Runs;

		Core::LayerStack m_LayerStack;

	private:
		ApplicationData m_AppData;

	private:
		bool OnClosedEvent(ME::Events::WindowClosedEvent& event);
		bool OnWindowSizeEvent(ME::Events::WindowResizedEvent& event);

	private:
		ME::Core::Memory::Scope<Window> m_Window;
		Render::Imgui::ImGuiLayer* m_ImGuiLayer;

	private:
		int32 m_WindowUpdateX;
		int32 m_WindowUpdateY;

	private:
		static Application* s_Instance;
		static bool s_ShutdownRequested;
	}
	;
	// Must be defined in client
	Application* CreateApplication();
	
}