#include "pch.h"
#include "Application.h"
#include "Core/Core.h"
#include "Core/Utils/Logging/Logger.h"
#include "Core\Events\MouseEvents.h"
#include "Core\Events\KeyEvents.h"
#include "Core/Utils/Benchmark/Benchmark.h"

namespace Pawn {

	Application::Application()
	{
	}

	void Application::Run()
	{
		KeyInputStartedEvent a(5, 10);
		PE_INFO(a.GetString());
		Vector2D res;
		{
			PE_INFO("Calcultation started");
			Vector2D f = { 0.46336, 1241.12867131 };
			PE_TRACE("f:		x = {}		y = {}",  f.x, f.y);
			Vector2D f1 = { 65.593652645, 12.74899126 };
			PE_TRACE("f1:		x = {}		y = {}", f1.x, f1.y);
			{
				Benchmark(nanoseconds, STRING("Vector2D ûגאûא test"));
				res = f + f1;
			}
			PE_INFO("Calcultation ended");
			PE_TRACE("f:		x = {}		y = {}", f.x, f.y);
			PE_TRACE("f1:		x = {}		y = {}", f1.x, f1.y);
			PE_TRACE("Result:	x = {}		y = {}", res.x, res.y);
		}
		
		while (true);
	}

}