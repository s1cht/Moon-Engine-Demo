#pragma once
#include "Core.hpp"
#include "Core/Misc/Time.hpp"

namespace ME::Core::Clock
{
	class COREAPI WindowsTimeSource : public TimeSource
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