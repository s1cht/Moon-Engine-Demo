module;

#include "Core.h"

export module Pawn.Core.Container.StringShared;

import Pawn.Core;

export namespace Pawn
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