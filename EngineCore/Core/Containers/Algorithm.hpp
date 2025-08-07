#pragma once

#include "Core.hpp"

namespace ME::Core::Algorithm
{
	template<typename Iterator, typename T>
	Iterator Find(Iterator first, Iterator last, const T& value)
	{
		for (Iterator it = first; it != last; ++it)
			if (*it == value)
				return it;
		return last;
	}

	template<typename Iterator, typename T>
	Iterator BinaryFind(Iterator first, Iterator last, const T& value)
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

	template<typename FirstIterator, typename SecondIterator>
	FirstIterator FindFirstOf(FirstIterator first, FirstIterator last,
		SecondIterator firstF, SecondIterator lastF)
	{
		for (; first != last; ++first)
			for (SecondIterator it = firstF; it != lastF; ++it)
				if (*first == *it)
					return first;
		return last;
	}

}