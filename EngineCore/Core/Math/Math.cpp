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

    Frustum ExtractFrustumFromMatrix(const Matrix4x4& vpMatrix)
    {
		Frustum frustum = {};

		// left
		frustum.Planes[0].x = vpMatrix.a[0][3] + vpMatrix.a[0][0];
		frustum.Planes[0].y = vpMatrix.a[1][3] + vpMatrix.a[1][0];
		frustum.Planes[0].z = vpMatrix.a[2][3] + vpMatrix.a[2][0];
		frustum.Planes[0].w = vpMatrix.a[3][3] + vpMatrix.a[3][0];

		// right
		frustum.Planes[1].x = vpMatrix.a[0][3] - vpMatrix.a[0][0];
		frustum.Planes[1].y = vpMatrix.a[1][3] - vpMatrix.a[1][0];
		frustum.Planes[1].z = vpMatrix.a[2][3] - vpMatrix.a[2][0];
		frustum.Planes[1].w = vpMatrix.a[3][3] - vpMatrix.a[3][0];

		// bottom
		frustum.Planes[2].x = vpMatrix.a[0][3] + vpMatrix.a[0][1];
		frustum.Planes[2].y = vpMatrix.a[1][3] + vpMatrix.a[1][1];
		frustum.Planes[2].z = vpMatrix.a[2][3] + vpMatrix.a[2][1];
		frustum.Planes[2].w = vpMatrix.a[3][3] + vpMatrix.a[3][1];

		// top
		frustum.Planes[3].x = vpMatrix.a[0][3] - vpMatrix.a[0][1];
		frustum.Planes[3].y = vpMatrix.a[1][3] - vpMatrix.a[1][1];
		frustum.Planes[3].z = vpMatrix.a[2][3] - vpMatrix.a[2][1];
		frustum.Planes[3].w = vpMatrix.a[3][3] - vpMatrix.a[3][1];

		// near
		frustum.Planes[4].x = vpMatrix.a[0][3] + vpMatrix.a[0][2];
		frustum.Planes[4].y = vpMatrix.a[1][3] + vpMatrix.a[1][2];
		frustum.Planes[4].z = vpMatrix.a[2][3] + vpMatrix.a[2][2];
		frustum.Planes[4].w = vpMatrix.a[3][3] + vpMatrix.a[3][2];

		// far
		frustum.Planes[5].x = vpMatrix.a[0][3] - vpMatrix.a[0][2];
		frustum.Planes[5].y = vpMatrix.a[1][3] - vpMatrix.a[1][2];
		frustum.Planes[5].z = vpMatrix.a[2][3] - vpMatrix.a[2][2];
		frustum.Planes[5].w = vpMatrix.a[3][3] - vpMatrix.a[3][2];

		// normalize all planes
		for (uint8 i = 0; i < 6; i++)
			frustum.Planes[i] /= Vector3D(frustum.Planes[i].x, frustum.Planes[i].y, frustum.Planes[i].z).Length();

        return frustum;
    }

	inline float64 Lerp(float64 a, float64 b, float64 d)
	{
		return LERP(a, b, d);
	}
	inline float32 Lerp(float32 a, float32 b, float32 d)
	{
		return LERP(a, b, d);
	}

    float32 ToDegrees(float32 radians)
	{
		return static_cast<float32>(ToDegrees(static_cast<float64>(radians)));
	}

    float64 ToDegrees(float64 radians)
    {
        return radians / (PI / 180);
    }

	float32 ToRadians(float32 degrees)
	{
		return static_cast<float32>(ToRadians(static_cast<float64>(degrees)));
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