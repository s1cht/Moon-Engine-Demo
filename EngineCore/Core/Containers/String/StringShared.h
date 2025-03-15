#pragma once

#include "StringMacros.h"
#include "Core.h"

namespace Pawn
{
	template<typename T>
	CORE_API SIZE_T GetStringSize(const T* str)
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