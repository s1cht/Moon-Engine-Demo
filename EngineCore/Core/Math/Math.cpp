#include "Math.hpp"

namespace ME::Core::Math
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
	// Vector3 statics // ---------------------------------------
	///////////////////// ---------------------------------------

	template<> const Vector3<float32> Vector3<float32>::ZeroVector(0);
	template<> const Vector3<float32> Vector3<float32>::UpVector(0, 1, 0);
	template<> const Vector3<float32> Vector3<float32>::DownVector(0, -1, 0);
	template<> const Vector3<float32> Vector3<float32>::ForwardVector(0, 0, 1);
	template<> const Vector3<float32> Vector3<float32>::BackwardVector(0, 0, -1);
	template<> const Vector3<float32> Vector3<float32>::RightVector(-1, 0, 0);
	template<> const Vector3<float32> Vector3<float32>::LeftVector(1, 0, 0);

	template<> const Vector3<float64> Vector3<float64>::ZeroVector(0);
	template<> const Vector3<float64> Vector3<float64>::UpVector(0, 1, 0);
	template<> const Vector3<float64> Vector3<float64>::DownVector(0, -1, 0);
	template<> const Vector3<float64> Vector3<float64>::ForwardVector(0, 0, 1);
	template<> const Vector3<float64> Vector3<float64>::BackwardVector(0, 0, -1);
	template<> const Vector3<float64> Vector3<float64>::RightVector(-1, 0, 0);
	template<> const Vector3<float64> Vector3<float64>::LeftVector(1, 0, 0);

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