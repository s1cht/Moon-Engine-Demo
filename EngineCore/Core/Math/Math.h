#pragma once

#include "Vector2D.h"
#include "Vector3D.h"
#include "Vector4D.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"
#include "Formulas.h"
#include "Constants.h"

#define IMPL_VEC_CONSTANTS(vector, var, ...) 	template<typename T> vector<T> const vector<T>::var = vector<T>(__VA_ARGS__);