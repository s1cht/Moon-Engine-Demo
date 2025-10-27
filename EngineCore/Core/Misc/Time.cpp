#include "Time.hpp"

namespace ME::Core::Clock
{
	Memory::Scope<TimeSource> Time::s_Source = nullptr;
	Timepoint Time::s_LastFrame{};
	Timestep Time::s_DeltaTime{};
	uint32 Time::s_InstantFPS = 0;
	uint32 Time::s_AverageFPS = 0;
	float64 Time::s_FPSAccumulatedTime = 0.0;
	uint32 Time::s_FPSFrameCount = 0;

	void Time::Init()
	{
		s_Source = TimeSource::Create();
		s_LastFrame = Timepoint::Now(s_Source.get());
		s_DeltaTime = Timestep(0, s_Source.get());
		s_InstantFPS = 0;
		s_AverageFPS = 0;
		s_FPSAccumulatedTime = 0.0;
		s_FPSFrameCount = 0;
	}

	void Time::Shutdown()
	{
		s_Source.reset();
	}

	void Time::UpdateFPS()
	{
		float64 deltaSeconds = s_DeltaTime.AsSeconds();
		s_InstantFPS = static_cast<uint32>(1.0 / deltaSeconds);

		s_FPSAccumulatedTime += deltaSeconds;
		s_FPSFrameCount++;

		if (s_FPSAccumulatedTime >= 1.0)
		{
			s_AverageFPS = static_cast<uint32>(static_cast<float64>(s_FPSFrameCount) / s_FPSAccumulatedTime);
			s_FPSAccumulatedTime = 0.0;
			s_FPSFrameCount = 0;
		}
	}

	Timestep Time::Update()
	{
		ME_ASSERT(s_Source, "TimeSource not initialized!");
		Timepoint current = Timepoint::Now(s_Source.get());
		s_DeltaTime = Timestep(s_LastFrame, current);
		s_LastFrame = current;

		UpdateFPS();
		return s_DeltaTime;
	}

	ME::Core::Clock::Timestep Time::GetDeltaTime()
	{
		return s_DeltaTime;
	}

	float64 Time::GetDeltaSeconds()
	{
		return s_DeltaTime.AsSeconds();
	}

	uint32 Time::GetInstantFPS()
	{
		return s_InstantFPS;
	}

	uint32 Time::GetAverageFPS()
	{
		return s_AverageFPS;
	}

	StructurizedTime ConvertTickToTime(uint64 tick, uint64 frequency)
	{
		StructurizedTime time{};
		float64 seconds = static_cast<float64>(tick) / frequency;

		time.Year = static_cast<uint16>(seconds / TickConstants::Values::Year);
		seconds -= time.Year * TickConstants::Values::Year;

		time.Weeks = static_cast<uint8>(seconds / TickConstants::Values::Week);
		seconds -= time.Weeks * TickConstants::Values::Week;

		time.Days = static_cast<uint16>(seconds / TickConstants::Values::Day);
		seconds -= time.Days * TickConstants::Values::Day;

		time.Hours = static_cast<uint8>(seconds / TickConstants::Values::Hour);
		seconds -= time.Hours * TickConstants::Values::Hour;

		time.Minutes = static_cast<uint8>(seconds / TickConstants::Values::Minute);
		seconds -= time.Minutes * TickConstants::Values::Minute;

		time.Seconds = static_cast<uint8>(seconds);
		seconds -= time.Seconds;

		time.Milliseconds = static_cast<uint16>(seconds * 1000);
		seconds -= time.Milliseconds / 1000.0;

		time.Microseconds = static_cast<uint16>(seconds * 1e6);
		seconds -= time.Microseconds / 1e6;

		time.Nanoseconds = static_cast<uint16>(seconds * 1e9);

		return time;
	}

#ifndef PLATFORM_WINDOWS
	Memory::Scope<TimeSource> TimeSource::Create()
	{
		return nullptr;
	}
#endif
}