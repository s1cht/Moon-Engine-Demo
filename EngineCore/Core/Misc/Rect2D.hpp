#pragma once

#include "Core.hpp"
#include "Core/Math/Vector2.hpp"

namespace Pawn::Core::Math
{
	template<typename T>
	struct NumberPair
	{
		T x;
		T y;
	};

	struct Rect2D
	{
	public:
		NumberPair<int32> Offset;
		NumberPair<uint32> Extent;
	};
}
