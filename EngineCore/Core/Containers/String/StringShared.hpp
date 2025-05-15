#pragma once

#include "Core.hpp"

namespace Pawn::Core::Containers
{
	template<typename T>
	SIZE_T GetStringSize(const T* str)
	{
		SIZE_T size = 0;
		while (*str != '\0')
		{
			++size;
			++str;
		}
		return size;
	}

}