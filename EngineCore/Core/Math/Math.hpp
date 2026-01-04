#pragma once
#include "Core/Math/Vector2.hpp"
#include "Core/Math/Vector3.hpp"
#include "Core/Math/Vector4.hpp"
#include "Core/Math/Quaternion.hpp"
#include "Core/Math/Matrix3x3.hpp"
#include "Core/Math/Matrix4x4.hpp"
#include "Core/Math/Color.hpp"
#include "Core/Algorithm.hpp"

namespace ME::Core::Math
{
	constexpr const float64 Pi = PI;
	constexpr const float64 Epsilon = 2.2204460492503131e-016;

	// Vector2
	typedef Vector2<float32> Vector2D32;
	typedef Vector2<float64> Vector2D64;
	typedef Vector2D32 Vector2D;

	// Vector3
	typedef Vector3<float32> Vector3D32;
	typedef Vector3<float64> Vector3D64;
	typedef Vector3D32 Vector3D;

	// Vector4
	typedef Vector4<float32> Vector4D32;
	typedef Vector4<float64> Vector4D64;
	typedef Vector4D32 Vector4D;

	struct Frustum
	{
		Vector4D Planes[6];
	};

	COREAPI Frustum ExtractFrustumFromMatrix(const Matrix4x4& vpMatrix);

	inline COREAPI float64 Lerp(float64 a, float64 b, float64 d);
	inline COREAPI float32 Lerp(float32 a, float32 b, float32 d);

	template<typename T>
	T Clamp(T min, T max, T val)
	{
		return Algorithm::Max(min, Algorithm::Min(val, max));
	}

	inline COREAPI float32 ToDegrees(float32 radians);
	inline COREAPI float64 ToDegrees(float64 radians);
	inline COREAPI float32 ToRadians(float32 degrees);
	inline COREAPI float64 ToRadians(float64 degrees);
}

inline ME::Core::Math::Vector4D operator*(const ME::Core::Math::Vector4D& vector, const ME::Core::Math::Matrix4x4& matrix)
{
	return ME::Core::Math::Vector4D(vector.x * matrix.m11 + vector.y * matrix.m21 + vector.z * matrix.m31 + vector.w * matrix.m41,
		vector.x * matrix.m12 + vector.y * matrix.m22 + vector.z * matrix.m32 + vector.w * matrix.m42,
		vector.x * matrix.m13 + vector.y * matrix.m23 + vector.z * matrix.m33 + vector.w * matrix.m43,
		vector.x * matrix.m14 + vector.y * matrix.m24 + vector.z * matrix.m34 + vector.w * matrix.m44
	);
}

ME_FMT_FORMATTER(ME::Core::Math::Vector2D32, "{}, {}", ME_FMT_FORMATTER_VALUE(x), ME_FMT_FORMATTER_VALUE(y));
ME_FMT_FORMATTER(ME::Core::Math::Matrix4x4, "Row 0: {}, {}, {}, {}\nRow 1: {}, {}, {}, {}\nRow 2: {}, {}, {}, {}\nRow 3: {}, {}, {}, {}", 
	ME_FMT_FORMATTER_VALUE(m11), ME_FMT_FORMATTER_VALUE(m12), ME_FMT_FORMATTER_VALUE(m13), ME_FMT_FORMATTER_VALUE(m14),
	ME_FMT_FORMATTER_VALUE(m21), ME_FMT_FORMATTER_VALUE(m22), ME_FMT_FORMATTER_VALUE(m23), ME_FMT_FORMATTER_VALUE(m24),
	ME_FMT_FORMATTER_VALUE(m31), ME_FMT_FORMATTER_VALUE(m32), ME_FMT_FORMATTER_VALUE(m33), ME_FMT_FORMATTER_VALUE(m34),
	ME_FMT_FORMATTER_VALUE(m41), ME_FMT_FORMATTER_VALUE(m42), ME_FMT_FORMATTER_VALUE(m43), ME_FMT_FORMATTER_VALUE(m44));
ME_FMT_FORMATTER(ME::Core::Math::Vector2D64, "{}, {}", ME_FMT_FORMATTER_VALUE(x), ME_FMT_FORMATTER_VALUE(y));
ME_FMT_FORMATTER(ME::Core::Math::Vector3D32, "{}, {}, {}", ME_FMT_FORMATTER_VALUE(x), ME_FMT_FORMATTER_VALUE(y), ME_FMT_FORMATTER_VALUE(z));
ME_FMT_FORMATTER(ME::Core::Math::Vector3D64, "{}, {}, {}", ME_FMT_FORMATTER_VALUE(x), ME_FMT_FORMATTER_VALUE(y), ME_FMT_FORMATTER_VALUE(z));
ME_FMT_FORMATTER(ME::Core::Math::Vector4D32, "{}, {}, {}, {}", ME_FMT_FORMATTER_VALUE(x), ME_FMT_FORMATTER_VALUE(y), ME_FMT_FORMATTER_VALUE(z), ME_FMT_FORMATTER_VALUE(w));
ME_FMT_FORMATTER(ME::Core::Math::Vector4D64, "{}, {}, {}, {}", ME_FMT_FORMATTER_VALUE(x), ME_FMT_FORMATTER_VALUE(y), ME_FMT_FORMATTER_VALUE(z), ME_FMT_FORMATTER_VALUE(w));
ME_FMT_FORMATTER(ME::Core::Math::Quaternion, "{}, {}, {}, {}", ME_FMT_FORMATTER_VALUE(w), ME_FMT_FORMATTER_VALUE(x), ME_FMT_FORMATTER_VALUE(y), ME_FMT_FORMATTER_VALUE(z));