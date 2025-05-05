module Pawn.Core.Math;

#include "Core.h"
#include "MathMacros.h"

import Pawn.Core;
import Pawn.Core.Math.Matrix3x3;
import Pawn.Core.Math.Matrix4x4;
import Pawn.Core.Math.Quaternion;
import Pawn.Core.Math.Vector2;
import Pawn.Core.Math.Vector3;
import Pawn.Core.Math.Vector4;

namespace Pawn::Core::Math
{
	///////////////////// ---------------------------------------
	// Vector2 statics // ---------------------------------------
	///////////////////// ---------------------------------------
	template<> const Vector2<float32> Vector2<float32>::ZeroVector(0);
	template<> const Vector2<float32> Vector2<float32>::UpVector(0, 1);
	template<> const Vector2<float32> Vector2<float32>::DownVector(0, -1);
	template<> const Vector2<float32> Vector2<float32>::LeftVector(-1, 0);
	template<> const Vector2<float32> Vector2<float32>::RightVector(1, 0);

	template<> const Vector2<float64> Vector2<float64>::ZeroVector(0);
	template<> const Vector2<float64> Vector2<float64>::UpVector(0, 1);
	template<> const Vector2<float64> Vector2<float64>::DownVector(0, -1);
	template<> const Vector2<float64> Vector2<float64>::LeftVector(-1, 0);
	template<> const Vector2<float64> Vector2<float64>::RightVector(1, 0);

	///////////////////// ---------------------------------------
	// Vector4 statics // ---------------------------------------
	///////////////////// ---------------------------------------
	template<> const Vector4<float32> Vector4<float32>::ZeroVector(0);
	template<> const Vector4<float32> Vector4<float32>::OneVector(1);

	template<> const Vector4<float64> Vector4<float64>::ZeroVector(0);
	template<> const Vector4<float64> Vector4<float64>::OneVector(1);

	inline float64 Lerp(float64 a, float64 b, float64 d)
	{
		return LERP(a, b, d);
	}
	inline float32 Lerp(float32 a, float32 b, float32 d)
	{
		return LERP(a, b, d);
	}

	constexpr const float64 P_PI = PI;
}

#undef IMPL_VEC_CONSTANTS