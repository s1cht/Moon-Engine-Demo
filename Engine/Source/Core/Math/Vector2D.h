#pragma once

#include "Core/Core.h"
#include "Core/String/String.h"
#include <type_traits>

namespace Pawn 
{
	template<typename T>
	struct Vector2
	{
		union
		{
			struct
			{
				T x, y;
			};
			T val[2];
		};

		Vector2<T> operator+ (const Vector2& b)
		{
			//Vector2 result;
			this->x += b.x;
			this->y += b.y;

			return *this;
		}

	};

	// Default Vector2D. If you want use another type, then use Vector2<type>
	typedef Pawn::Vector2<float64> Vector2D;

};