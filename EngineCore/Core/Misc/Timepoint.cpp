#include "Timepoint.h"

namespace Pawn::Time
{
	StructurizedTime ConvertTickToTime(uint64 tick)
	{
		using namespace TickConstants::Values;

		StructurizedTime returnTime = {};
		uint32 buf = (uint32)(tick % Year);
		
		returnTime.Year = (uint16)buf;
		tick -= Year * buf;

		buf = (uint32)(tick % Week);
		returnTime.Weeks = (uint8)buf;

		buf = (uint32)(tick % Day);
		returnTime.Days = (uint16)buf;
		tick -= Day * buf;

		buf = (uint32)(tick % Hour);
		returnTime.Hours = (uint8)buf;
		tick -= Hour * buf;

		buf = (uint32)(tick % Minute);
		returnTime.Minutes = (uint8)buf;
		tick -= Minute * buf;

		buf = (uint32)(tick % Second);
		returnTime.Seconds = (uint8)buf;
		tick -= Second * buf;

		buf = (uint32)(tick % Millisecond);
		returnTime.Milliseconds = (uint16)buf;
		tick -= Millisecond * buf;

		buf = (uint32)(tick % Microsecond);
		returnTime.Microseconds = (uint16)buf;
		tick -= Microsecond * buf;

		returnTime.Nanoseconds = (uint16)((float64)tick / Nanosecond);

		return returnTime;
	}
}
