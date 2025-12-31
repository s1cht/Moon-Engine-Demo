#include "WindowsTime.hpp"

#ifdef PLATFORM_WINDOWS
namespace ME::Core::Clock
{
	WindowsTimeSource::WindowsTimeSource()
	{
	}

	uint64 WindowsTimeSource::GetTicks() const
	{
		LARGE_INTEGER ticks;
		QueryPerformanceCounter(&ticks);
		return static_cast<uint64>(ticks.QuadPart);
	}

	uint64 WindowsTimeSource::GetFrequency() const
	{
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);
		return static_cast<uint64>(frequency.QuadPart);
	}


	Memory::Scope<TimeSource> TimeSource::Create()
	{
		return Memory::Scope<TimeSource>(new WindowsTimeSource());
	}

}
#endif
