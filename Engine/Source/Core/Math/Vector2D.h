//
//					Vector2D.h
//		Part of Pawn Engines math library
//

#pragma once

#include "pch.h"
#include "Core/Core.h"
#include "Core/String/String.h"
#include "Core/Math/Constants.h"
#include "Core/Math/Formulas.h"

/*													*/
/*		Vector2 operator implementation macros		*/
/*													*/


// Arithmetic calculation function implementation with number
#define _VECTOR2_OPERATOR_IMPL(_operator, type)\
		Vector2<T> operator##_operator (const type& b) const\
		{\
			Vector2 result;\
			result.x = this->x _operator (T)b;\
			result.y = this->y _operator (T)b;\
			return result;\
		}

// Arithmetic calculation function implementation with Vector2
#define _VECTOR2_VEC_OPERATOR_IMPL(_operator)\
		Vector2<T> operator##_operator (const Vector2& b)\
		{\
			Vector2 result;\
			result.x = this->x _operator (T)b.x;\
			result.y = this->y _operator (T)b.y;\
			return result;\
		}

// Arithmetic calculation function implementation with number and self
#define _VECTOR2_OPERATOR_IMPL_EQUAL(_operator, type)\
		Vector2<T>& operator##_operator= (const type& b)\
		{\
			this->x _operator= (T)b;\
			this->y _operator= (T)b;\
			return *this;\
		}

// Arithmetic calculation function implementation with Vector2 and self
#define _VECTOR2_VEC_OPERATOR_IMPL_EQUAL(_operator)\
		Vector2<T>& operator##_operator= (const Vector2& b)\
		{\
			this->x _operator= (T)b.x;\
			this->y _operator= (T)b.y;\
			return *this;\
		}

#define VECTOR2_OPERATOR_IMPL(_operator)								\
		_VECTOR2_VEC_OPERATOR_IMPL				(_operator);			\
		_VECTOR2_OPERATOR_IMPL					(_operator, int8);		\
		_VECTOR2_OPERATOR_IMPL					(_operator, int16);		\
		_VECTOR2_OPERATOR_IMPL					(_operator, int32);		\
		_VECTOR2_OPERATOR_IMPL					(_operator, int64);		\
		_VECTOR2_OPERATOR_IMPL					(_operator, float32);	\
		_VECTOR2_OPERATOR_IMPL					(_operator, float64);	\

#define VECTOR2_OPERATOR_IMPL_EQ(_operator)								\
		_VECTOR2_VEC_OPERATOR_IMPL_EQUAL		(_operator);			\
		_VECTOR2_OPERATOR_IMPL_EQUAL			(_operator, int8);		\
		_VECTOR2_OPERATOR_IMPL_EQUAL			(_operator, int16);		\
		_VECTOR2_OPERATOR_IMPL_EQUAL			(_operator, int32);		\
		_VECTOR2_OPERATOR_IMPL_EQUAL			(_operator, int64);		\
		_VECTOR2_OPERATOR_IMPL_EQUAL			(_operator, float32);	\
		_VECTOR2_OPERATOR_IMPL_EQUAL			(_operator, float64);	\

namespace Pawn::Math
{

	template<typename T>
	struct PAWN_API Vector2
	{

		union
		{
			struct
			{
				T x, y;
			};
			T val[2];
		};

		// Operators
		
		Vector2<T>& operator= (const Vector2& b)
		{
			this->x = (T)b.x;
			this->y = (T)b.y;
			return *this;
		}

		bool operator== (const Vector2& b)
		{
			return (this->x == (T)b.x && this->y == (T)b.y);
		}

		_VECTOR2_VEC_OPERATOR_IMPL(+);				// +
		_VECTOR2_VEC_OPERATOR_IMPL(-);				// -
		VECTOR2_OPERATOR_IMPL(*);					// *
		VECTOR2_OPERATOR_IMPL(/);					// /

		_VECTOR2_VEC_OPERATOR_IMPL_EQUAL(+);		// +=
		_VECTOR2_VEC_OPERATOR_IMPL_EQUAL(-);		// -=
		VECTOR2_OPERATOR_IMPL_EQ(*);				// *=
		VECTOR2_OPERATOR_IMPL_EQ(/);				// /=

		// Normalize vector
		Vector2<T> Normalize() const
		{
			T len = Length();
			return (*this) / len;
		}

		Vector2<T> Lerp(Vector2& vec, float32 t)
		{
			return LERP((*this), vec, t);
		}

		// Get angle (in radians) between vectors
		float64 GetAngleBetweenVectors(const Vector2& other) const
		{
			return std::acos(this->Dot(other) / (this->Length() * other.Length()));
		}

		// Get angle (in radians) between vectors
		float64 GetAngleBetweenVectorsInDegrees(const Vector2& other) const
		{
			return std::acos(this->Dot(other) / (this->Length() * other.Length())) * ((float64)180 / PI);
		}

		// Dot product of two vectors
		inline float64 Dot(const Vector2& other) const
		{
			return (float64)this->x * (float64)other.x + (float64)this->y * (float64)other.y;
		}

		// Preudo dot product of two vectors
		inline float64 Cross(const Vector2& other) const
		{
			return (float64)this->x * (float64)other.x - (float64)this->y * (float64)other.y;
		}

		// Length of vector
		inline float64 Length() const
		{
			return std::sqrt(SQUARE_SUM_2((float64)this->x, (float64)this->y));
		}

		// Squared length
		inline float64 LengthSquared() const
		{
			return SQUARE_SUM_2((float64)this->x, (float64)this->y);
		}

	};

	// Default Vector2D. If you want use another type, then use Vector2<type>
	typedef Vector2<float64> Vector2D;

};