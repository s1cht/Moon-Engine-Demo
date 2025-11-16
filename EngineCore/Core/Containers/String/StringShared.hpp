#pragma once
#include "Core/Types.hpp"

namespace ME::Core
{
	template<typename T>
	constexpr SIZE_T GetStringSize(const T* str)
	{
		SIZE_T size = 0;
		while (*str != 0)
		{
			++size;
			++str;
		}
		return size;
	}

}