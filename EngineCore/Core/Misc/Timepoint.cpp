#include "Timepoint.h"

namespace Pawn::Time
{
	StructurizedTime ConvertTickToTime(uint64 tick)
	{
		using namespace Tick::Values;

		StructurizedTime returnTime = {};
		int32 buf = tick % Year;
		
		returnTime.Year = (uint16)buf;
		tick -= Year * buf;

		buf = tick % Week;
		returnTime.Weeks = (uint8)buf;

		buf = tick % Day;
		returnTime.Days = (uint16)buf;
		tick -= Day * buf;

		buf = tick % Hour;
		returnTime.Hours = (uint8)buf;
		tick -= Hour * buf;

		buf = tick % Minute;
		returnTime.Minutes = (uint8)buf;
		tick -= Minute * buf;

		buf = tick % Second;
		returnTime.Seconds = (uint8)buf;
		tick -= Second * buf;

		buf = tick % Millisecond;
		returnTime.Milliseconds = (uint16)buf;
		tick -= Millisecond * buf;

		buf = tick % Microsecond;
		returnTime.Microseconds = (uint16)buf;
		tick -= Microsecond * buf;

		returnTime.Nanoseconds = (uint16)tick / Nanosecond;

		return returnTime;
	}
}
