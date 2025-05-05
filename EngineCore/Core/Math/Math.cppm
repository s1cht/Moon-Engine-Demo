module;

export module Pawn.Core.Math;

import Pawn.Core;
import Pawn.Core.Math.Matrix3x3;
import Pawn.Core.Math.Matrix4x4;
import Pawn.Core.Math.Quaternion;
import Pawn.Core.Math.Vector2;
import Pawn.Core.Math.Vector3;
import Pawn.Core.Math.Vector4;

export namespace Pawn::Core::Math
{

	inline float64 Lerp(float64 a, float64 b, float64 d);
	inline float32 Lerp(float32 a, float32 b, float32 d);

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
}