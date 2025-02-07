#include "pch.h"
#include "Application.h"
#include "Core/Core.h"
#include "Core/Utils/Logging/Logger.h"
#include "Core\Events\MouseEvents.h"
#include "Core\Events\KeyEvents.h"
#include "Core/Utils/Benchmark/Benchmark.h"
#include "Core/Utils/MemWatch/MemWatch.h"
#include "Core/Math/Vector2D.h"
#include "Core/Math/Vector3D.h"
#include "Core/String/String.h"
#include "Core/Containers/Array.h"

#include <vector>

namespace Pawn {

	using namespace std::chrono_literals;
	using namespace Math;


	Application::Application()
	{
	}

	void Application::Run()
	{
	#define ITER_COUNT1 10000000
	#define ITER_COUNT2 500000
	#define ITER_COUNT3 2500000

		{
			Array<Vector3<char8>, 10> array;

			Array<Vector3<char8>>::ReturnType element = array[5];

		}
		bool works = true;

		while (works)
		{
			//Pawn::MemWatch::OnUpdate();
		};
	}

}