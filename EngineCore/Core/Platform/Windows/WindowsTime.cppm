module;

export module Pawn.Core.Windows.Time;

import Pawn.Core.Clock;
import Pawn.Core.Windows;

namespace Pawn::Core::Clock
{
	class CORE_API WindowsTimeSource : public TimeSource
	{
	public:
		WindowsTimeSource();

	public:
		uint64 GetTicks() const override;
		uint64 GetFrequency() const override;
			
	private:
		LARGE_INTEGER m_Frequency;
	};
}