#pragma once
#include "Core/Core.h"
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
		std::unique_ptr<Input> m_Input;

	private:
		LayerStack m_LayerStack;

	};
	// Must be defined in client
	Application* CreateApplication();
	
}