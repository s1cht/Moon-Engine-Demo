#include "pch.h"
#include "StringShared.h"

namespace Pawn
{
	PAWN_API SIZE_T GetStringSize(const uchar* str)
    {
        SIZE_T size = 0;
        while (*str != TEXT('\0'))
        {
            ++size;
            ++str;
        }
        return size;
    }


}