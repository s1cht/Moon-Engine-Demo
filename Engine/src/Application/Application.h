#pragma once
#include "src/Core/Core.h"

namespace Pawn {

	class ENGINE_API Application
	{
	public:
		Application();
		virtual ~Application() = default;

	public:
		void Run();

	};

	// Must be defined in client
	Application* CreateApplication();

}