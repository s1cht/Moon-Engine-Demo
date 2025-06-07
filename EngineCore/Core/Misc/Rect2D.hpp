#pragma once

#include "Core.hpp"
#include "Core/Math/Vector2.hpp"

namespace Pawn::Core::Math
{
	template<typename T>
	struct Resolution2D
	{
		T x;
		T y;
	};

	struct Rect2D
	{
	public:
		Resolution2D<int32> Offset;
		Resolution2D<uint32> Extent;
	};


}
