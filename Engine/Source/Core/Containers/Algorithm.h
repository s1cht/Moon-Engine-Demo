#pragma once

#include "Core/Core.h"
#include "Core/Misc/CoreAssertion.h"

namespace Pawn
{
	namespace Algorithm
	{
		template<typename Iterator, typename T>
		PAWN_API Iterator Find(Iterator first, Iterator last, const T& value)
		{
			for (Iterator it = first; it != last; ++it)
			{
				if (*it == value)
					return it;
			}
			return last;
		}

		template<typename Iterator, typename T>
		PAWN_API Iterator BinaryFind(Iterator first, Iterator last, const T& value)
		{
			while (first < last)
			{
				Iterator mid = first + (last - first) / 2;

				if (*mid == value)
					return mid;
				else if (*mid < value)
					first = mid + 1;
				else
					last = mid;
			}

			return last;
		}
	}
}