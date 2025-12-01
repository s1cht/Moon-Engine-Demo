#pragma once
#include <Core.hpp>
#include <Core/Layer/LayerStack.hpp>
#include <Core/Events/Event.hpp>
#include <Core/Memory/Memory.hpp>

#include "Audio/AudioEngine.hpp"
#include "ECS/World.hpp"
#include "Input/Input.hpp"
#include "Platform/Platform.hpp"
#include "Renderer/ImGui/ImGuiLayer.hpp"

#define ME_GET_SCENE() ME::Application::Get().GetScene()

namespace ME 
{
	struct MEAPI ApplicationProperties
	{
		int32 VersionMajor; 
		int32 VersionMinor;
		int32 VersionPatch;
		ME::Core::String ApplicationName;

		ME::Core::Array<ME::Core::String> AssetPaths;
		ME::Core::Array<ME::Render::Manager::ShaderGroupSpecification> ShaderPaths;

		ApplicationProperties(int32 verMajor = 1, int32 verMinor = 0, int32 verPatch = 0, ME::Core::String appName = TEXT("Application"))
			: VersionMajor(verMajor), VersionMinor(verMinor), VersionPatch(verPatch), ApplicationName(appName) {}
	};

	typedef ApplicationProperties ApplicationData;

	class MEAPI Application
	{
	public:
		Application(ApplicationProperties props = ApplicationProperties());
		~Application();

	public:
		static Application& Get() { return *s_Instance; }
		static ME::Core::Memory::Reference<ME::ECS::World> GetWorld() { return Get().m_World; }
		static Render::ImGui::ImGuiLayer* GetImguiLayer() { return Get().m_ImGuiLayer; }
		static const ApplicationData& GetAppData() { return Get().m_AppData; }
		static Window* GetWindow() { return Get().m_Window.get(); }
		static ME::Core::Memory::Scope<ME::Audio::AudioEngine>& GetAudioEngine() { return Get().m_AudioEngine; }

		static void Run() { Get().RunImpl(); }
	    static void RequestShutdown() { s_ShutdownRequested = true; }

		static void PushLayer(Core::Layer* layer);
		static void PushOverlay(Core::Layer* overlay);

	public:
		void OnEvent(Core::Event& event);

	private:
		void RunImpl();

	private:
		bool OnClosedEvent(ME::Events::WindowClosedEvent& event);
		bool OnWindowSizeEvent(ME::Events::WindowResizedEvent& event);

	private:
		void LoadAssets();

	private:
		bool m_Runs;

		Core::LayerStack m_LayerStack;

	private:
		ApplicationData m_AppData;
		ME::Core::Memory::Reference<ME::ECS::World> m_World;

	private:
		ME::Core::Memory::Scope<Window> m_Window;
		ME::Core::Memory::Scope<Audio::AudioEngine> m_AudioEngine;
		Render::ImGui::ImGuiLayer* m_ImGuiLayer;

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