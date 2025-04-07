module Pawn.Core.Math;

#include "Vector2D.h"
#include "Vector3D.h"
#include "Vector4D.h"
#include "MathMacros.h"

#define IMPL_VEC_CONSTANTS(vector, var, ...) 	vector const vector::var = vector(__VA_ARGS__);

namespace Pawn::Core::Math
{
	///////////////////// ---------------------------------------
	// Vector2 statics // ---------------------------------------
	///////////////////// ---------------------------------------
	IMPL_VEC_CONSTANTS(Vector2D32, ZeroVector, 0);
	IMPL_VEC_CONSTANTS(Vector2D32, UpVector,			0,		1);
	IMPL_VEC_CONSTANTS(Vector2D32, DownVector,			0,		-1);
	IMPL_VEC_CONSTANTS(Vector2D32, LeftVector,			-1,		0);
	IMPL_VEC_CONSTANTS(Vector2D32, RightVector,		1,		0);

	IMPL_VEC_CONSTANTS(Vector2D64, ZeroVector, 0);
	IMPL_VEC_CONSTANTS(Vector2D64, UpVector, 0, 1);
	IMPL_VEC_CONSTANTS(Vector2D64, DownVector, 0, -1);
	IMPL_VEC_CONSTANTS(Vector2D64, LeftVector, -1, 0);
	IMPL_VEC_CONSTANTS(Vector2D64, RightVector, 1, 0);

	///////////////////// ---------------------------------------
	// Vector3 statics // ---------------------------------------
	///////////////////// ---------------------------------------
	IMPL_VEC_CONSTANTS(Vector3D32, ZeroVector, 0);
	IMPL_VEC_CONSTANTS(Vector3D32, UpVector,			0,		1,		0);
	IMPL_VEC_CONSTANTS(Vector3D32, DownVector,			0,		-1,		0);
	IMPL_VEC_CONSTANTS(Vector3D32, ForwardVector,		0,		0,		1);
	IMPL_VEC_CONSTANTS(Vector3D32, BackwardVector,		0,		0,		-1);
	IMPL_VEC_CONSTANTS(Vector3D32, RightVector,		1,		0,		0);
	IMPL_VEC_CONSTANTS(Vector3D32, LeftVector,			-1,		0,		0);

	IMPL_VEC_CONSTANTS(Vector3D64, ZeroVector, 0);
	IMPL_VEC_CONSTANTS(Vector3D64, UpVector, 0, 1, 0);
	IMPL_VEC_CONSTANTS(Vector3D64, DownVector, 0, -1, 0);
	IMPL_VEC_CONSTANTS(Vector3D64, ForwardVector, 0, 0, 1);
	IMPL_VEC_CONSTANTS(Vector3D64, BackwardVector, 0, 0, -1);
	IMPL_VEC_CONSTANTS(Vector3D64, RightVector, 1, 0, 0);
	IMPL_VEC_CONSTANTS(Vector3D64, LeftVector, -1, 0, 0);

	///////////////////// ---------------------------------------
	// Vector4 statics // ---------------------------------------
	///////////////////// ---------------------------------------
	IMPL_VEC_CONSTANTS(Vector4D32, ZeroVector, 0);
	IMPL_VEC_CONSTANTS(Vector4D32, OneVector, 1);

	IMPL_VEC_CONSTANTS(Vector4D64, ZeroVector, 0);
	IMPL_VEC_CONSTANTS(Vector4D64, OneVector, 1);

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