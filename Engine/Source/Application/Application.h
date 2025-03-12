#pragma once
#include "Core.h"
#include "Core/Layer/LayerStack.h"
#include "Platform/Platform.h"
#include "Input/Input.h"
#include "Renderer/ImGui/ImGuiLayer.h"
#include "Events/WindowEvents.h"

namespace Pawn {

	class PAWN_API Application
	{
	public:
		Application();
		virtual ~Application();

	public:
		Window* GetWindow() { return m_Window.get(); }

	public:
		void Run();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

	public:
		void OnEvent(Event& event);

	public:
		static Application& Get() { return *s_Instance; }

	private:
		bool m_Runs;

		LayerStack m_LayerStack;
	private:
		bool OnWindowSizeEvent(WindowResizedEvent& event);

	private:
		Pawn::Memory::Scope<Window> m_Window;
		Render::Imgui::ImGuiLayer* m_ImGuiLayer;

	private:
		int32 m_WindowUpdateX;
		int32 m_WindowUpdateY;

	private:
		static Application* s_Instance;

	};
	// Must be defined in client
	Application* CreateApplication();
	
}