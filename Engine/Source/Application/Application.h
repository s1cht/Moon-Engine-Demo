#pragma once
#include "Core/Core.h"
#include "Core/Math/Vector2D.h"

namespace Pawn {

	class PAWN_API Application
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