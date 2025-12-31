#pragma once
#include "Core.hpp"
#include "Core/Time.hpp"

#ifdef PLATFORM_WINDOWS
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
#endif