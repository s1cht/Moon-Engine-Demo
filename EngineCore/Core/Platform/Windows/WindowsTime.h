#pragma once

#include "Core/Misc/Time.h"

namespace Pawn::Time
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