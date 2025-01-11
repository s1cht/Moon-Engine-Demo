#include "pch.h"
#include "Application.h"
#include "Events\MouseEvents.h"
#include "Events\KeyEvents.h"

namespace Pawn {

	Application::Application()
	{
	}

	void Application::Run()
	{
		KeyInputStartedEvent a(5, 10);
		PE_INFO(a.GetString());
		while (true);
	}

}