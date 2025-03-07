#pragma once
#include "Core.h"
#include "Core/Layer/LayerStack.h"
#include "Platform/Platform.h"
#include "Input/Input.h"

namespace Pawn {

	class PAWN_API Application
	{
	public:
		Application();
		virtual ~Application() = default;

	public:
		void Run();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

	public:
		void OnEvent(Event& event);


	private:
		bool m_Runs;

	private:
		std::unique_ptr<Window> m_Window;

	private:
		LayerStack m_LayerStack;

	};
	// Must be defined in client
	Application* CreateApplication();
	
}