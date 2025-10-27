#include "WindowsTime.hpp"

namespace ME::Core::Clock
{
	WindowsTimeSource::WindowsTimeSource()
	{
		QueryPerformanceFrequency(&m_Frequency);
	}

	uint64 WindowsTimeSource::GetTicks() const
	{
		LARGE_INTEGER ticks;
		QueryPerformanceCounter(&ticks);
		return static_cast<uint64>(ticks.QuadPart);
	}

	uint64 WindowsTimeSource::GetFrequency() const
	{
		return static_cast<uint64>(m_Frequency.QuadPart);
	}

#ifdef PLATFORM_WINDOWS

	Memory::Scope<TimeSource> TimeSource::Create()
	{
		return Memory::Scope<TimeSource>(new WindowsTimeSource());
	}

#endif
}
