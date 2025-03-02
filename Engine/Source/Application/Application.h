#pragma once
#include "Core/Core.h"
#include "Core/Math/Vector2D.h"
#include "Platform/Platform.h"

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

		bool m_Runs;
	};
	// Must be defined in client
	Application* CreateApplication();
	
}