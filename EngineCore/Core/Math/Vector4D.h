//
//					Vector4D.h
//		Part of Pawn Engines math library
//

#pragma once

#include "pch.h"
#include "Core.h"

/*													*/
/*		Vector4 operator implementation macros		*/
/*													*/


// Arithmetic calculation function implementation with number								
#define _VECTOR4_OPERATOR_IMPL(_operator, type)												\
		template<typename T>																\
		Vector4<T> Pawn::Math::Vector4<T>::operator##_operator (const type& b) const		\
		{																					\
			Vector4 result;																	\
			result.x = this->x _operator (T)b;												\
			result.y = this->y _operator (T)b;												\
			result.z = this->z _operator (T)b;												\
			result.w = this->w _operator (T)b;												\
			return result;																	\
		}																					

// Arithmetic calculation function implementation with Vector4								
#define _VECTOR4_VEC_OPERATOR_IMPL(_operator)												\
		template<typename T>																\
		Vector4<T> Pawn::Math::Vector4<T>::operator##_operator (const Vector4& b) const		\
		{																					\
			Vector4 result;																	\
			result.x = this->x _operator (T)b.x;											\
			result.y = this->y _operator (T)b.y;											\
			result.z = this->z _operator (T)b.z;											\
			result.w = this->w _operator (T)b.w;											\
			return result;																	\
		}																					

// Arithmetic calculation function implementation with number and self						
#define _VECTOR4_OPERATOR_IMPL_EQUAL(_operator, type)										\
		template<typename T>																\
		Vector4<T>& Pawn::Math::Vector4<T>::operator##_operator= (const type& b)			\
		{																					\
			this->x _operator= (T)b;														\
			this->y _operator= (T)b;														\
			this->z _operator= (T)b;														\
			this->w _operator= (T)b;														\
			return *this;																	\
		}																					

// Arithmetic calculation function implementation with Vector4 and self						
#define _VECTOR4_VEC_OPERATOR_IMPL_EQUAL(_operator)											\
		template<typename T>																\
		Vector4<T>& Pawn::Math::Vector4<T>::operator##_operator= (const Vector4& b)			\
		{																					\
			this->x _operator= (T)b.x;														\
			this->y _operator= (T)b.y;														\
			this->z _operator= (T)b.z;														\
			this->w _operator= (T)b.w;														\
			return *this;																	\
		}																					

#define VECTOR4_OPERATOR_IMPL(_operator)													\
		_VECTOR4_VEC_OPERATOR_IMPL				(_operator);								\
		_VECTOR4_OPERATOR_IMPL					(_operator, int8);							\
		_VECTOR4_OPERATOR_IMPL					(_operator, int16);							\
		_VECTOR4_OPERATOR_IMPL					(_operator, int32);							\
		_VECTOR4_OPERATOR_IMPL					(_operator, int64);							\
		_VECTOR4_OPERATOR_IMPL					(_operator, float32);						\
		_VECTOR4_OPERATOR_IMPL					(_operator, float64);						

#define VECTOR4_OPERATOR_IMPL_EQ(_operator)													\
		_VECTOR4_VEC_OPERATOR_IMPL_EQUAL		(_operator);								\
		_VECTOR4_OPERATOR_IMPL_EQUAL			(_operator, int8);							\
		_VECTOR4_OPERATOR_IMPL_EQUAL			(_operator, int16);							\
		_VECTOR4_OPERATOR_IMPL_EQUAL			(_operator, int32);							\
		_VECTOR4_OPERATOR_IMPL_EQUAL			(_operator, int64);							\
		_VECTOR4_OPERATOR_IMPL_EQUAL			(_operator, float32);						\
		_VECTOR4_OPERATOR_IMPL_EQUAL			(_operator, float64);						

// Arithmetic calculation function definition with number									
#define _VECTOR4_OPERATOR_DEF(_operator, type)												\
		Vector4<T> operator##_operator (const type& b) const								

// Arithmetic calculation function definition with number and self							
#define _VECTOR4_OPERATOR_DEF_EQUAL(_operator, type)										\
		Vector4<T>& operator##_operator= (const type& b)									

#define VECTOR4_OPERATOR_DEF(_operator)														\
		_VECTOR4_OPERATOR_DEF					(_operator, Vector4);						\
		_VECTOR4_OPERATOR_DEF					(_operator, int8);							\
		_VECTOR4_OPERATOR_DEF					(_operator, int16);							\
		_VECTOR4_OPERATOR_DEF					(_operator, int32);							\
		_VECTOR4_OPERATOR_DEF					(_operator, int64);							\
		_VECTOR4_OPERATOR_DEF					(_operator, float32);						\
		_VECTOR4_OPERATOR_DEF					(_operator, float64);						

#define VECTOR4_OPERATOR_DEF_EQUAL(_operator)												\
		_VECTOR4_OPERATOR_DEF_EQUAL				(_operator, Vector4);						\
		_VECTOR4_OPERATOR_DEF_EQUAL				(_operator, int8);							\
		_VECTOR4_OPERATOR_DEF_EQUAL				(_operator, int16);							\
		_VECTOR4_OPERATOR_DEF_EQUAL				(_operator, int32);							\
		_VECTOR4_OPERATOR_DEF_EQUAL				(_operator, int64);							\
		_VECTOR4_OPERATOR_DEF_EQUAL				(_operator, float32);						\
		_VECTOR4_OPERATOR_DEF_EQUAL				(_operator, float64);						

template<typename T>
struct Vector2;

namespace Pawn::Math
{
	template<typename T>
	struct Vector4
	{

		union
		{
			struct
			{
				T x, y, z, w;
			};
			struct
			{
				T X, Y, Z, W;
			};
			T XYZ[4];
		};

	public:

		Vector4();

		Vector4(T scalar);

		Vector4(T _x, T _y, T _z, T _w);

		Vector4(Vector2<T> vec1, Vector2<T> vec2);

		Vector4(Vector3<T> vec, T _w);

		Vector4(const Vector4& otherVec);

		Vector4(Vector4&& otherVec) noexcept;

		~Vector4();

		// ...
	public:

		// Operators

		Vector4<T>& operator= (const Vector4& b);

		inline bool operator== (const Vector4& b) const;

		_VECTOR4_OPERATOR_DEF(+, Vector4);
		_VECTOR4_OPERATOR_DEF(-, Vector4);
		VECTOR4_OPERATOR_DEF(*);
		VECTOR4_OPERATOR_DEF(/ );

		_VECTOR4_OPERATOR_DEF_EQUAL(+, Vector4);
		_VECTOR4_OPERATOR_DEF_EQUAL(-, Vector4);
		VECTOR4_OPERATOR_DEF_EQUAL(*);
		VECTOR4_OPERATOR_DEF_EQUAL(/ );

		// Normalize vector
		Vector4<T> Normalize() const;

		Vector4<T> Lerp(Vector4& vec, float32 t) const;

		// Length of vector
		inline float64 Length() const;

		// Squared length
		inline float64 LengthSquared() const;

	};

	template<typename T>
	inline Vector4<T>::Vector4()
		: x((T)0.f), y((T)0.f), z((T)0.f), w((T)0.f)
	{
	}

	template<typename T>
	inline Vector4<T>::Vector4(T scalar)
		: x(scalar), y(scalar) 
	{
	}

	template<typename T>
	inline Vector4<T>::Vector4(T _x, T _y, T _z, T _w)
		: x(_x), y(_y), z(_z), w(_w) {
	}

	template<typename T>
	inline Vector4<T>::Vector4(Vector2<T> vec1, Vector2<T> vec2)
		: x(vec1.X), y(vec1.Y), z(vec2.X), w(vec2.Y) {
	}

	template<typename T>
	inline Vector4<T>::Vector4(Vector3<T> vec, T _w)
		: x(vec.X), y(vec.Y), z(vec.Z), w(_w) {
	}

	template<typename T>
	inline Vector4<T>::Vector4(const Vector4& otherVec)
	{
		x = otherVec.x;
		y = otherVec.y;
		z = otherVec.z;
		w = otherVec.w;
	}

	template<typename T>
	inline Vector4<T>::Vector4(Vector4&& otherVec) noexcept
	{
		x = otherVec.x;
		y = otherVec.y;
		z = otherVec.z;
		w = otherVec.w;
	}

	template<typename T>
	inline Vector4<T>::~Vector4()
	{
	}

	template<typename T>
	Vector4<T>& Pawn::Math::Vector4<T>::operator= (const Vector4& b)
	{
		this->x = (T)b.x;
		this->y = (T)b.y;
		this->z = (T)b.z;
		this->w = (T)b.w;
		return *this;
	}

	template<typename T>
	inline bool Pawn::Math::Vector4<T>::operator== (const Vector4& b) const
	{
		return (this->x == (T)b.x && this->y == (T)b.y && this->z == (T)b.z && this->w == (T)b.w);
	}

	_VECTOR4_VEC_OPERATOR_IMPL(+);
	_VECTOR4_VEC_OPERATOR_IMPL(-);
	VECTOR4_OPERATOR_IMPL(*);
	VECTOR4_OPERATOR_IMPL(/ );

	_VECTOR4_VEC_OPERATOR_IMPL_EQUAL(+);
	_VECTOR4_VEC_OPERATOR_IMPL_EQUAL(-);
	VECTOR4_OPERATOR_IMPL_EQ(*);
	VECTOR4_OPERATOR_IMPL_EQ(/ );

	template<typename T>
	Vector4<T> Pawn::Math::Vector4<T>::Normalize() const
	{
		T len = Length();
		return (*this) / len;
	}

	template<typename T>
	Vector4<T> Pawn::Math::Vector4<T>::Lerp(Vector4& vec, float32 t) const
	{
		return LERP((*this), vec, t);
	}

	template<typename T>
	float64 Pawn::Math::Vector4<T>::Length() const
	{
		return std::sqrt(SQUARE_SUM_4((float64)this->x, (float64)this->y, (float64)this->z, (float64)this->w));
	}

	template<typename T>
	float64 Pawn::Math::Vector4<T>::LengthSquared() const
	{
		return SQUARE_SUM_4((float64)this->x, (float64)this->y, (float64)this->z, (float64)this->w);
	}

	// Default Vector3D. If you want use another type, then use Vector2<type>
	typedef Vector4<float32> Vector4D32;
	typedef Vector4<float64> Vector4D64;

	typedef Vector4D64 Vector4D;

};

#undef _VECTOR4_OPERATOR_IMPL
#undef _VECTOR4_VEC_OPERATOR_IMPL
#undef _VECTOR4_OPERATOR_IMPL_EQUAL
#undef _VECTOR4_VEC_OPERATOR_IMPL_EQUAL
#undef VECTOR4_OPERATOR_IMPL
#undef VECTOR4_OPERATOR_IMPL_EQ
#undef _VECTOR4_OPERATOR_DEF
#undef _VECTOR4_OPERATOR_DEF_EQUAL
#undef VECTOR4_OPERATOR_DEF
#undef VECTOR4_OPERATOR_DEF_EQ