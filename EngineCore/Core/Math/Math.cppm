module;

#include "Vector2D.h"
#include "Vector3D.h"
#include "Vector4D.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"

export module Pawn.Core.Math;

export namespace Pawn::Core::Math
{
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
}