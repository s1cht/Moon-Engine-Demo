#pragma once

#include "Core.hpp"
#include "Core/Math/Vector2.hpp"

namespace ME::Core::Math
{
	template<typename T>
	struct Resolution2D
	{
		T x;
		T y;
	};

	template<typename T>
	struct Resolution3D
	{
		T x;
		T y;
		T z;
	};

	struct Rect2D
	{
		Resolution2D<int32> Offset;
		Resolution2D<uint32> Extent;
	};
}
