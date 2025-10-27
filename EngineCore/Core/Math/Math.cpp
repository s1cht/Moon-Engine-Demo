#include "Math.hpp"

namespace ME::Core::Math
{
	///////////////////// ---------------------------------------
	// Vector2 statics // ---------------------------------------
	///////////////////// ---------------------------------------
	
	template<> const Vector2<float32> Vector2<float32>::ZeroVector(0);
	template<> const Vector2<float32> Vector2<float32>::UpVector(0, 1);
	template<> const Vector2<float32> Vector2<float32>::DownVector(0, -1);
	template<> const Vector2<float32> Vector2<float32>::RightVector(1, 0);
	template<> const Vector2<float32> Vector2<float32>::LeftVector(-1, 0);

	template<> const Vector2<float64> Vector2<float64>::ZeroVector(0);
	template<> const Vector2<float64> Vector2<float64>::UpVector(0, 1);
	template<> const Vector2<float64> Vector2<float64>::DownVector(0, -1);
	template<> const Vector2<float64> Vector2<float64>::RightVector(1, 0);
	template<> const Vector2<float64> Vector2<float64>::LeftVector(-1, 0);

	///////////////////// ---------------------------------------
	// Vector3 statics // ---------------------------------------
	///////////////////// ---------------------------------------

	template<> const Vector3<float32> Vector3<float32>::ZeroVector(0); 
	template<> const Vector3<float32> Vector3<float32>::UpVector(0, 1, 0);
	template<> const Vector3<float32> Vector3<float32>::DownVector(0, -1, 0);
	template<> const Vector3<float32> Vector3<float32>::RightVector(1, 0, 0);
	template<> const Vector3<float32> Vector3<float32>::LeftVector(-1, 0, 0);
	template<> const Vector3<float32> Vector3<float32>::ForwardVector(0, 0, 1);
	template<> const Vector3<float32> Vector3<float32>::BackwardVector(0, 0,-1);

	template<> const Vector3<float64> Vector3<float64>::ZeroVector(0);
	template<> const Vector3<float64> Vector3<float64>::UpVector(0, 1, 0);
	template<> const Vector3<float64> Vector3<float64>::DownVector(0, -1, 0);
	template<> const Vector3<float64> Vector3<float64>::RightVector(1, 0, 0);
	template<> const Vector3<float64> Vector3<float64>::LeftVector(-1, 0, 0);
	template<> const Vector3<float64> Vector3<float64>::ForwardVector(0, 0, 1);
	template<> const Vector3<float64> Vector3<float64>::BackwardVector(0, 0, -1);

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

    float64 ToDegrees(float64 radians)
    {
        return radians / (PI / 180);
    }

	float64 ToRadians(float64 degrees)
	{
		return RAD(degrees);
	}

    std::ostream& operator<<(std::ostream& os, const Vector2D32& vec)
	{
		return os << vec.x << ' ' << vec.y;
	}

	std::ostream& operator<<(std::ostream& os, const Vector2D64& vec)
	{
		return os << vec.x << ' ' << vec.y;
	}

	std::ostream& operator<<(std::ostream& os, const Vector3D32& vec)
	{
		return os << vec.x << ' ' << vec.y << ' ' << vec.z;
	}

	std::ostream& operator<<(std::ostream& os, const Vector3D64& vec)
	{
		return os << vec.x << ' ' << vec.y << ' ' << vec.z;
	}

	std::ostream& operator<<(std::ostream& os, const Vector4D32& vec)
	{
		return os << vec.x << ' ' << vec.y << ' ' << vec.z << ' ' << vec.w;
	}

	std::ostream& operator<<(std::ostream& os, const Vector4D64& vec)
	{
		return os << vec.x << ' ' << vec.y << ' ' << vec.z << ' ' << vec.w;
	}

	std::wostream& operator<<(std::wostream& os, const Vector2D32& vec)
	{
		return os << vec.x << L' ' << vec.y;
	}

	std::wostream& operator<<(std::wostream& os, const Vector2D64& vec)
	{
		return os << vec.x << L' ' << vec.y;
	}

	std::wostream& operator<<(std::wostream& os, const Vector3D32& vec)
	{
		return os << vec.x << L' ' << vec.y << L' ' << vec.z;
	}

	std::wostream& operator<<(std::wostream& os, const Vector3D64& vec)
	{
		return os << vec.x << L' ' << vec.y << L' ' << vec.z;
	}

	std::wostream& operator<<(std::wostream& os, const Vector4D32& vec)
	{
		return os << vec.x << L' ' << vec.y << L' ' << vec.z << L' ' << vec.w;
	}

	std::wostream& operator<<(std::wostream& os, const Vector4D64& vec)
	{
		return os << vec.x << L' ' << vec.y << L' ' << vec.z << L' ' << vec.w;
	}
}

#undef IMPL_VEC_CONSTANTS