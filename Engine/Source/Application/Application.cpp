#include "pch.h"
#include "Application.h"
#include "Core/Core.h"
#include "Core/Utils/Logging/Logger.h"
#include "Core\Events\MouseEvents.h"
#include "Core\Events\KeyEvents.h"
#include "Core/Utils/Benchmark/Benchmark.h"
#include "Core/Utils/MemWatch/MemWatch.h"
#include "Core/Math/Vector3D.h"
#include "Core/Containers/Array.h"

namespace Pawn {

	using namespace std::chrono_literals;

	Application::Application()
	{
	}

	void Application::Run()
	{
		int b = 124;
		Array<int32, 4> array = {123412, 1293819, 1263891, 1351, 121926319};

		array.PushBack(154);
		array.EmplaceBack(1123);
		
		Math::Vector3D a = {0.15125, 12516, 125125.2437};

		bool works = true;

		while (works)
		{
			//Pawn::MemWatch::OnUpdate();
		};
	}

}