#pragma once

#include "Core/Types.hpp"
#include "Core/Math/MathMacros.hpp"
#include "Core/Math/Vector2.hpp"
#include "Core/Math/Vector3.hpp"
#include "Core/Math/Vector4.hpp"
#include "Core/Math/Quaternion.hpp"
#include "Core/Math/Matrix3x3.hpp"
#include "Core/Math/Matrix4x4.hpp"

namespace ME::Core::Math
{
	constexpr const float64 P_PI = PI;

	// Vector2
	typedef Vector2<float32> Vector2D32;
	typedef Vector2<float64> Vector2D64;
	typedef Vector2D64 Vector2D;

	// Vector3
	typedef Vector3<float32> Vector3D32;
	typedef Vector3<float64> Vector3D64;
	typedef Vector3D64 Vector3D;

	// Vector4
	typedef Vector4<float32> Vector4D32;
	typedef Vector4<float64> Vector4D64;
	typedef Vector4D64 Vector4D;

	typedef PMatrix3x3 Matrix3x3;
	typedef PMatrix4x4 Matrix4x4;
	typedef PQuaternion Quaternion;

	inline float64 Lerp(float64 a, float64 b, float64 d);
	inline float32 Lerp(float32 a, float32 b, float32 d);
}

ME_FMT_FORMATTER(ME::Core::Math::Vector2D32, "{}, {}", ME_FMT_FORMATTER_VALUE(x), ME_FMT_FORMATTER_VALUE(y));
ME_FMT_FORMATTER(ME::Core::Math::Vector2D64, "{}, {}", ME_FMT_FORMATTER_VALUE(x), ME_FMT_FORMATTER_VALUE(y));
ME_FMT_FORMATTER(ME::Core::Math::Vector3D32, "{}, {}, {}", ME_FMT_FORMATTER_VALUE(x), ME_FMT_FORMATTER_VALUE(y), ME_FMT_FORMATTER_VALUE(z));
ME_FMT_FORMATTER(ME::Core::Math::Vector3D64, "{}, {}, {}", ME_FMT_FORMATTER_VALUE(x), ME_FMT_FORMATTER_VALUE(y), ME_FMT_FORMATTER_VALUE(z));
ME_FMT_FORMATTER(ME::Core::Math::Vector4D32, "{}, {}, {}, {}", ME_FMT_FORMATTER_VALUE(x), ME_FMT_FORMATTER_VALUE(y), ME_FMT_FORMATTER_VALUE(z), ME_FMT_FORMATTER_VALUE(w));
ME_FMT_FORMATTER(ME::Core::Math::Vector4D64, "{}, {}, {}, {}", ME_FMT_FORMATTER_VALUE(x), ME_FMT_FORMATTER_VALUE(y), ME_FMT_FORMATTER_VALUE(z), ME_FMT_FORMATTER_VALUE(w));