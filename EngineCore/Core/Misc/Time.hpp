#pragma once

#include "Core.hpp"
#include "Core/Utils/Logging/Logger.hpp"
#include "Core/Memory/Memory.hpp"

namespace ME::Core::Clock
{
	class CORE_API TimeSource
	{
	public:
		virtual ~TimeSource() = default;
		virtual uint64 GetTicks() const = 0;
		virtual uint64 GetFrequency() const = 0;

	public:
		static Memory::Scope<TimeSource> Create();
	};

	namespace TickConstants
	{
		inline constexpr int64 Max = 0xFFFFFFFFFFFFFFFF;
		inline constexpr int64 Min = 0x0000000000000000;

		namespace Values
		{
			inline constexpr float64 Nanosecond = 0.001;
			inline constexpr uint64 Microsecond = 10;
			inline constexpr uint64 Millisecond = Microsecond * 1000;
			inline constexpr uint64 Second = Millisecond * 1000;
			inline constexpr uint64 Minute = Second * 60;
			inline constexpr uint64 Hour = Minute * 60;
			inline constexpr uint64 Day = Hour * 24;
			inline constexpr uint64 Week = Day * 7;
			inline constexpr uint64 Year = Day * 365;
		}
	}

	struct StructurizedTime
	{
		uint16 Year;
		uint8 Weeks;
		uint16 Days;
		uint8 Hours;
		uint8 Minutes;
		uint8 Seconds;
		uint16 Milliseconds;
		uint16 Microseconds;
		uint16 Nanoseconds;
	};

	extern CORE_API StructurizedTime ConvertTickToTime(uint64 tick, uint64 frequency);

	struct CORE_API Timepoint
	{
	public:
		Timepoint()
			: Tick(0), Source(nullptr)
		{
		}

		Timepoint(uint64 ticks, TimeSource* source)
			: Tick(ticks), Source(source)
		{
			ME_ASSERT(ticks <= TickConstants::Max && ticks >= TickConstants::Min, TEXT("Tick limit exceed!"));
		}

		StructurizedTime GetTime() const
		{
			ME_ASSERT(Source, TEXT("TimeSource not set!"));
			return ConvertTickToTime(Tick, Source->GetFrequency());
		}

		static Timepoint Now(TimeSource* source)
		{
			ME_ASSERT(source, TEXT("TimeSource not set!"));
			return Timepoint(source->GetTicks(), source);
		}

	public:
		uint64 Tick;
		TimeSource* Source;
	};

	class Timestep
	{
	public:
		Timestep()
			: m_Ticks(0), m_Source(nullptr)
		{
		}

		Timestep(uint64 ticks, TimeSource* source)
			: m_Ticks(ticks), m_Source(source)
		{
		}

		Timestep(const Timepoint& start, const Timepoint& end)
			: m_Ticks(end.Tick - start.Tick), m_Source(end.Source)
		{
			ME_ASSERT(end.Source == start.Source, TEXT("Timepoints must use the same TimeSource!"));
			ME_ASSERT(end.Tick >= start.Tick, TEXT("End time must be greater than or equal to start time!"));
		}

	public:
		float64 AsNanoseconds() const
		{
			ME_ASSERT(m_Source, TEXT("TimeSource not set!"));
			return (static_cast<float64>(m_Ticks) / m_Source->GetFrequency()) * 1e9;
		}
		float64 AsMicroseconds() const
		{
			ME_ASSERT(m_Source, TEXT("TimeSource not set!"));
			return (static_cast<float64>(m_Ticks) / m_Source->GetFrequency()) * 1e6;
		}
		float64 AsMilliseconds() const
		{
			ME_ASSERT(m_Source, TEXT("TimeSource not set!"));
			return (static_cast<float64>(m_Ticks) / m_Source->GetFrequency()) * 1e3;
		}
		float64 AsSeconds() const
		{
			ME_ASSERT(m_Source, TEXT("TimeSource not set!"));
			return static_cast<float64>(m_Ticks) / m_Source->GetFrequency();
		}

		uint64 GetTicks() const { return m_Ticks; }
		StructurizedTime GetStructurizedTime() const
		{
			ME_ASSERT(m_Source, TEXT("TimeSource not set!"));
			return ConvertTickToTime(m_Ticks, m_Source->GetFrequency());
		}

		Timestep operator+(const Timestep& other) const
		{
			ME_ASSERT(m_Source == other.m_Source, TEXT("Timesteps must use the same TimeSource!"));
			return Timestep(m_Ticks + other.m_Ticks, m_Source);
		}
		Timestep operator-(const Timestep& other) const
		{
			ME_ASSERT(m_Source == other.m_Source, TEXT("Timesteps must use the same TimeSource!"));
			ME_ASSERT(m_Ticks >= other.m_Ticks, TEXT("Timestep subtraction would result in negative value!"));
			return Timestep(m_Ticks - other.m_Ticks, m_Source);
		}
		Timestep& operator+=(const Timestep& other)
		{
			ME_ASSERT(m_Source == other.m_Source, TEXT("Timesteps must use the same TimeSource!"));
			m_Ticks += other.m_Ticks;
			return *this;
		}
		Timestep& operator-=(const Timestep& other)
		{
			ME_ASSERT(m_Source == other.m_Source, TEXT("Timesteps must use the same TimeSource!"));
			ME_ASSERT(m_Ticks >= other.m_Ticks, TEXT("Timestep subtraction would result in negative value!"));
			m_Ticks -= other.m_Ticks;
			return *this;
		}
		bool operator==(const Timestep& other) const { return m_Ticks == other.m_Ticks && m_Source == other.m_Source; }
		bool operator!=(const Timestep& other) const { return !(*this == other); }

	private:
		uint64 m_Ticks;
		TimeSource* m_Source;
	};

	class CORE_API Time
	{
	public:
		static void Init();
		static void Shutdown();

		static Timestep Update();
		static Timestep GetDeltaTime(); 
		static float64 GetDeltaSeconds(); 
		static uint32 GetInstantFPS(); 
		static uint32 GetAverageFPS(); 

	private:
		static void UpdateFPS();

	private:
		static Memory::Scope<TimeSource> s_Source;
		static Timepoint s_LastFrame;
		static Timestep s_DeltaTime;
		static uint32 s_InstantFPS;
		static uint32 s_AverageFPS;
		static float64 s_FPSAccumulatedTime;
		static uint32 s_FPSFrameCount;
	};
}