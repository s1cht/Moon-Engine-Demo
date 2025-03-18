#pragma once

#include "Core.h"
#include "Core/Misc/Assertion.h"


namespace Pawn::Time
{	
	namespace TickConstants
	{
		inline constexpr int64 Max =						0xFFFFFFFFFFFFFFFF;
		inline constexpr int64 Min =						0x0000000000000000;

		namespace Values
		{
			inline constexpr float64	Nanosecond =		0.001;
			inline constexpr uint64		Microsecond =		10;
			inline constexpr uint64		Millisecond =		Microsecond * 1000;
			inline constexpr uint64		Second =			Millisecond * 1000;
			inline constexpr uint64		Minute =			Second * 60;
			inline constexpr uint64		Hour =				Minute * 60;
			inline constexpr uint64		Day =				Hour * 24;
			inline constexpr uint64		Week =				Day * 7;
			inline constexpr uint64		Year =				Day * 365;
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

	extern CORE_API StructurizedTime ConvertTickToTime(uint64 tick);

	struct Timepoint
	{

	public:
		Timepoint()
			: Tick(0)
		{
		}

		Timepoint(uint64 ticks)
			: Tick(ticks)
		{
			PE_ASSERT(!(ticks <= TickConstants::Max || ticks >= TickConstants::Min), TEXT("Tick limit exceed!"));
		}

		StructurizedTime GetTime()
		{
			return ConvertTickToTime(Tick);
		}


	public:
		uint64 Tick;

	};

}