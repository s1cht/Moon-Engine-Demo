module Pawn.Core.Math;

#include "Vector2D.h"
#include "Vector3D.h"
#include "Vector4D.h"


#define IMPL_VEC_CONSTANTS(vector, var, ...) 	template<typename T> vector<T> const vector<T>::var = vector<T>(__VA_ARGS__);

namespace Pawn::Core::Math
{
	///////////////////// ---------------------------------------
	// Vector2 statics // ---------------------------------------
	///////////////////// ---------------------------------------
	IMPL_VEC_CONSTANTS(Vector2, ZeroVector, 0);
	IMPL_VEC_CONSTANTS(Vector2, UpVector,			0,		1);
	IMPL_VEC_CONSTANTS(Vector2, DownVector,			0,		-1);
	IMPL_VEC_CONSTANTS(Vector2, LeftVector,			-1,		0);
	IMPL_VEC_CONSTANTS(Vector2, RightVector,		1,		0);

	///////////////////// ---------------------------------------
	// Vector3 statics // ---------------------------------------
	///////////////////// ---------------------------------------
	IMPL_VEC_CONSTANTS(Vector3, ZeroVector, 0);
	IMPL_VEC_CONSTANTS(Vector3, UpVector,			0,		1,		0);
	IMPL_VEC_CONSTANTS(Vector3, DownVector,			0,		-1,		0);
	IMPL_VEC_CONSTANTS(Vector3, ForwardVector,		0,		0,		1);
	IMPL_VEC_CONSTANTS(Vector3, BackwardVector,		0,		0,		-1);
	IMPL_VEC_CONSTANTS(Vector3, RightVector,		1,		0,		0);
	IMPL_VEC_CONSTANTS(Vector3, LeftVector,			-1,		0,		0);

}