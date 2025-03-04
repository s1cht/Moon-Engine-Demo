#pragma once
#include "Core/Core.h"
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

	public:
		void OnEvent(Event& event);

	private:
		std::unique_ptr<Window> m_Window;
		std::unique_ptr<Input> m_Input;

		bool m_Runs;
	};
	// Must be defined in client
	Application* CreateApplication();
	
}