#pragma once
#include "Core.hpp"
#include "Core/Time.hpp"

namespace ME::Core::Clock
{
	class COREAPI WindowsTimeSource : public TimeSource
	{
	public:
		WindowsTimeSource();

	public:
		uint64 GetTicks() const override;
		uint64 GetFrequency() const override;

	};
}