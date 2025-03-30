//
//					Vector3D.h
//		Part of Pawn Engines math library
//

#pragma once

#include "Core.h"
#include <math.h>


/*													*/
/*		Vector3 operator implementation macros		*/
/*													*/


// Arithmetic calculation function implementation with number								
#define _VECTOR3_OPERATOR_IMPL(_operator, type)												\
		template<typename T>																\
		Vector3<T> Pawn::Core::Math::Vector3<T>::operator##_operator (const type& b) const		\
		{																					\
			Vector3 result;																	\
			result.x = this->x _operator (T)b;												\
			result.y = this->y _operator (T)b;												\
			result.z = this->z _operator (T)b;												\
			return result;																	\
		}																					
																							
// Arithmetic calculation function implementation with Vector3								
#define _VECTOR3_VEC_OPERATOR_IMPL(_operator)												\
		template<typename T>																\
		Vector3<T> Pawn::Core::Math::Vector3<T>::operator##_operator (const Vector3& b) const		\
		{																					\
			Vector3 result;																	\
			result.x = this->x _operator (T)b.x;											\
			result.y = this->y _operator (T)b.y;											\
			result.z = this->z _operator (T)b.z;											\
			return result;																	\
		}																					
																							
// Arithmetic calculation function implementation with number and self						
#define _VECTOR3_OPERATOR_IMPL_EQUAL(_operator, type)										\
		template<typename T>																\
		Vector3<T>& Pawn::Core::Math::Vector3<T>::operator##_operator= (const type& b)			\
		{																					\
			this->x _operator= (T)b;														\
			this->y _operator= (T)b;														\
			this->z _operator= (T)b;														\
			return *this;																	\
		}																					
																							
// Arithmetic calculation function implementation with Vector3 and self						
#define _VECTOR3_VEC_OPERATOR_IMPL_EQUAL(_operator)											\
		template<typename T>																\
		Vector3<T>& Pawn::Core::Math::Vector3<T>::operator##_operator= (const Vector3& b)			\
		{																					\
			this->x _operator= (T)b.x;														\
			this->y _operator= (T)b.y;														\
			this->z _operator= (T)b.z;														\
			return *this;																	\
		}																					
																							
#define VECTOR3_OPERATOR_IMPL(_operator)													\
		_VECTOR3_VEC_OPERATOR_IMPL				(_operator);								\
		_VECTOR3_OPERATOR_IMPL					(_operator, int8);							\
		_VECTOR3_OPERATOR_IMPL					(_operator, int16);							\
		_VECTOR3_OPERATOR_IMPL					(_operator, int32);							\
		_VECTOR3_OPERATOR_IMPL					(_operator, int64);							\
		_VECTOR3_OPERATOR_IMPL					(_operator, float32);						\
		_VECTOR3_OPERATOR_IMPL					(_operator, float64);						
																							
#define VECTOR3_OPERATOR_IMPL_EQ(_operator)													\
		_VECTOR3_VEC_OPERATOR_IMPL_EQUAL		(_operator);								\
		_VECTOR3_OPERATOR_IMPL_EQUAL			(_operator, int8);							\
		_VECTOR3_OPERATOR_IMPL_EQUAL			(_operator, int16);							\
		_VECTOR3_OPERATOR_IMPL_EQUAL			(_operator, int32);							\
		_VECTOR3_OPERATOR_IMPL_EQUAL			(_operator, int64);							\
		_VECTOR3_OPERATOR_IMPL_EQUAL			(_operator, float32);						\
		_VECTOR3_OPERATOR_IMPL_EQUAL			(_operator, float64);						
																							
// Arithmetic calculation function definition with number									
#define _VECTOR3_OPERATOR_DEF(_operator, type)												\
		Vector3<T> operator##_operator (const type& b) const								
																							
// Arithmetic calculation function definition with number and self							
#define _VECTOR3_OPERATOR_DEF_EQUAL(_operator, type)										\
		Vector3<T>& operator##_operator= (const type& b)									
																							
#define VECTOR3_OPERATOR_DEF(_operator)														\
		_VECTOR3_OPERATOR_DEF					(_operator, Vector3);						\
		_VECTOR3_OPERATOR_DEF					(_operator, int8);							\
		_VECTOR3_OPERATOR_DEF					(_operator, int16);							\
		_VECTOR3_OPERATOR_DEF					(_operator, int32);							\
		_VECTOR3_OPERATOR_DEF					(_operator, int64);							\
		_VECTOR3_OPERATOR_DEF					(_operator, float32);						\
		_VECTOR3_OPERATOR_DEF					(_operator, float64);						
																							
#define VECTOR3_OPERATOR_DEF_EQUAL(_operator)												\
		_VECTOR3_OPERATOR_DEF_EQUAL				(_operator, Vector3);						\
		_VECTOR3_OPERATOR_DEF_EQUAL				(_operator, int8);							\
		_VECTOR3_OPERATOR_DEF_EQUAL				(_operator, int16);							\
		_VECTOR3_OPERATOR_DEF_EQUAL				(_operator, int32);							\
		_VECTOR3_OPERATOR_DEF_EQUAL				(_operator, int64);							\
		_VECTOR3_OPERATOR_DEF_EQUAL				(_operator, float32);						\
		_VECTOR3_OPERATOR_DEF_EQUAL				(_operator, float64);						


namespace Pawn::Core::Math
{
	template<typename T>
	struct Vector2;

	template<typename T>
	struct Vector3
	{

		union
		{
			struct
			{
				T x, y, z;
			};
			struct
			{
				T X, Y, Z;
			};
			T XYZ[3];
		};

	public:
		// (0, 0, 0)
		static const Vector3<T> ZeroVector;
		// (0, 1, 0)
		static const Vector3<T> UpVector;
		// (0, -1, 0)
		static const Vector3<T> DownVector;
		// (0, 0, 1)
		static const Vector3<T> ForwardVector;
		// (0, 0, -1)
		static const Vector3<T> BackwardVector;
		// (1, 0, 0)
		static const Vector3<T> RightVector;
		// (-1, 0, 0)
		static const Vector3<T> LeftVector;

	public:
		
		Vector3() = default;

		explicit Vector3(T scalar);

		Vector3(T _x, T _y, T _z);

		explicit Vector3(const Vector2<T> vec, T _z);

		Vector3(const Vector3& otherVec);

		Vector3(Vector3&& otherVec) noexcept;

		~Vector3();

		// ...
	public:

		// Operators

		Vector3<T>& operator= (const Vector3& b);

		inline bool operator== (const Vector3& b) const;

		Vector3<T> operator+ (const Vector3& b) const;
		Vector3<T> operator- (const Vector3& b) const;
		Vector3<T> operator* (const Vector3& b) const; 
		Vector3<T> operator* (const int8& b) const; 
		Vector3<T> operator* (const int16& b) const; 
		Vector3<T> operator* (const int32& b) const;
		Vector3<T> operator* (const int64& b) const; 
		Vector3<T> operator* (const float32& b) const; 
		Vector3<T> operator* (const float64& b) const;
		Vector3<T> operator/ (const Vector3& b) const; 
		Vector3<T> operator/ (const int8& b) const; 
		Vector3<T> operator/ (const int16& b) const;
		Vector3<T> operator/ (const int32& b) const;
		Vector3<T> operator/ (const int64& b) const; 
		Vector3<T> operator/ (const float32& b) const; 
		Vector3<T> operator/ (const float64& b) const;

		Vector3<T>& operator+= (const Vector3& b);
		Vector3<T>& operator-= (const Vector3& b);
		Vector3<T>& operator*= (const Vector3& b); 
		Vector3<T>& operator*= (const int8& b); 
		Vector3<T>& operator*= (const int16& b); 
		Vector3<T>& operator*= (const int32& b); 
		Vector3<T>& operator*= (const int64& b);
		Vector3<T>& operator*= (const float32& b); 
		Vector3<T>& operator*= (const float64& b);
		Vector3<T>& operator/= (const Vector3& b); 
		Vector3<T>& operator/= (const int8& b); 
		Vector3<T>& operator/= (const int16& b); 
		Vector3<T>& operator/= (const int32& b); 
		Vector3<T>& operator/= (const int64& b); 
		Vector3<T>& operator/= (const float32& b); 
		Vector3<T>& operator/= (const float64& b);

		// Normalize vector
		Vector3<T> Normalize() const;

		//
		Vector3<T> Lerp(Vector3& vec, float32 t) const;

		// Get angle (in radians) between vectors
		float64 GetAngleBetweenVectors(const Vector3& other) const;

		// Get angle (in radians) between vectors
		float64 GetAngleBetweenVectorsInDegrees(const Vector3& other) const;

		// Dot product of two vectors
		inline float64 Dot(const Vector3& other) const;

		// Preudo dot product of two vectors
		inline float64 Cross(const Vector3& other) const;

		// Length of vector
		inline float64 Length() const;

		// Squared length
		inline float64 LengthSquared() const;

	};

	template<typename T>
	inline Vector3<T>::Vector3(T scalar) 
		: x(scalar), y(scalar), z(scalar) {}

	template<typename T>
	inline Vector3<T>::Vector3(T _x, T _y, T _z)
		: x(_x), y(_y), z(_z) {}

	template<typename T>
	inline Vector3<T>::Vector3(Vector2<T> vec, T _z)
		: x(vec.x), y(vec.y), z(_z) {}

	template<typename T>
	inline Vector3<T>::Vector3(const Vector3& otherVec)
	{
		x = otherVec.x;
		y = otherVec.y;
		z = otherVec.z;
	}

	template<typename T>
	inline Vector3<T>::Vector3(Vector3&& otherVec) noexcept
	{
		x = otherVec.x;
		y = otherVec.y;
		z = otherVec.z;
	}

	template<typename T>
	inline Vector3<T>::~Vector3()
	{
	}

	template<typename T>
	Vector3<T>& Pawn::Core::Math::Vector3<T>::operator= (const Vector3& b)
	{
		this->x = (T)b.x;
		this->y = (T)b.y;
		this->z = (T)b.z;
		return *this;
	}

	template<typename T> 
	inline bool Pawn::Core::Math::Vector3<T>::operator== (const Vector3& b) const
	{
		return (this->x == (T)b.x && this->y == (T)b.y && this->z == (T)b.z);
	}

	template<typename T> Vector3<T> Pawn::Core::Math::Vector3<T>::operator+ (const Vector3& b) const {
		Vector3 result; result.x = this->x + (T)b.x; result.y = this->y + (T)b.y; result.z = this->z + (T)b.z; return result;
	};
	template<typename T> Vector3<T> Pawn::Core::Math::Vector3<T>::operator- (const Vector3& b) const {
		Vector3 result; result.x = this->x - (T)b.x; result.y = this->y - (T)b.y; result.z = this->z - (T)b.z; return result;
	};
	template<typename T> Vector3<T> Pawn::Core::Math::Vector3<T>::operator* (const Vector3& b) const {
		Vector3 result; result.x = this->x * (T)b.x; result.y = this->y * (T)b.y; result.z = this->z * (T)b.z; return result;
	}; template<typename T> Vector3<T> Pawn::Core::Math::Vector3<T>::operator* (const int8& b) const {
		Vector3 result; result.x = this->x * (T)b; result.y = this->y * (T)b; result.z = this->z * (T)b; return result;
	}; template<typename T> Vector3<T> Pawn::Core::Math::Vector3<T>::operator* (const int16& b) const {
		Vector3 result; result.x = this->x * (T)b; result.y = this->y * (T)b; result.z = this->z * (T)b; return result;
	}; template<typename T> Vector3<T> Pawn::Core::Math::Vector3<T>::operator* (const int32& b) const {
		Vector3 result; result.x = this->x * (T)b; result.y = this->y * (T)b; result.z = this->z * (T)b; return result;
	}; template<typename T> Vector3<T> Pawn::Core::Math::Vector3<T>::operator* (const int64& b) const {
		Vector3 result; result.x = this->x * (T)b; result.y = this->y * (T)b; result.z = this->z * (T)b; return result;
	}; template<typename T> Vector3<T> Pawn::Core::Math::Vector3<T>::operator* (const float32& b) const {
		Vector3 result; result.x = this->x * (T)b; result.y = this->y * (T)b; result.z = this->z * (T)b; return result;
	}; template<typename T> Vector3<T> Pawn::Core::Math::Vector3<T>::operator* (const float64& b) const {
		Vector3 result; result.x = this->x * (T)b; result.y = this->y * (T)b; result.z = this->z * (T)b; return result;
	};;
	template<typename T> Vector3<T> Pawn::Core::Math::Vector3<T>::operator/ (const Vector3& b) const {
		Vector3 result; result.x = this->x / (T)b.x; result.y = this->y / (T)b.y; result.z = this->z / (T)b.z; return result;
	}; template<typename T> Vector3<T> Pawn::Core::Math::Vector3<T>::operator/ (const int8& b) const {
		Vector3 result; result.x = this->x / (T)b; result.y = this->y / (T)b; result.z = this->z / (T)b; return result;
	}; template<typename T> Vector3<T> Pawn::Core::Math::Vector3<T>::operator/ (const int16& b) const {
		Vector3 result; result.x = this->x / (T)b; result.y = this->y / (T)b; result.z = this->z / (T)b; return result;
	}; template<typename T> Vector3<T> Pawn::Core::Math::Vector3<T>::operator/ (const int32& b) const {
		Vector3 result; result.x = this->x / (T)b; result.y = this->y / (T)b; result.z = this->z / (T)b; return result;
	}; template<typename T> Vector3<T> Pawn::Core::Math::Vector3<T>::operator/ (const int64& b) const {
		Vector3 result; result.x = this->x / (T)b; result.y = this->y / (T)b; result.z = this->z / (T)b; return result;
	}; template<typename T> Vector3<T> Pawn::Core::Math::Vector3<T>::operator/ (const float32& b) const {
		Vector3 result; result.x = this->x / (T)b; result.y = this->y / (T)b; result.z = this->z / (T)b; return result;
	}; template<typename T> Vector3<T> Pawn::Core::Math::Vector3<T>::operator/ (const float64& b) const {
		Vector3 result; result.x = this->x / (T)b; result.y = this->y / (T)b; result.z = this->z / (T)b; return result;
	};;

	template<typename T> Vector3<T>& Pawn::Core::Math::Vector3<T>::operator+= (const Vector3& b) {
		this->x += (T)b.x; this->y += (T)b.y; this->z += (T)b.z; return *this;
	};
	template<typename T> Vector3<T>& Pawn::Core::Math::Vector3<T>::operator-= (const Vector3& b) {
		this->x -= (T)b.x; this->y -= (T)b.y; this->z -= (T)b.z; return *this;
	};
	template<typename T> Vector3<T>& Pawn::Core::Math::Vector3<T>::operator*= (const Vector3& b) {
		this->x *= (T)b.x; this->y *= (T)b.y; this->z *= (T)b.z; return *this;
	}; template<typename T> Vector3<T>& Pawn::Core::Math::Vector3<T>::operator*= (const int8& b) {
		this->x *= (T)b; this->y *= (T)b; this->z *= (T)b; return *this;
	}; template<typename T> Vector3<T>& Pawn::Core::Math::Vector3<T>::operator*= (const int16& b) {
		this->x *= (T)b; this->y *= (T)b; this->z *= (T)b; return *this;
	}; template<typename T> Vector3<T>& Pawn::Core::Math::Vector3<T>::operator*= (const int32& b) {
		this->x *= (T)b; this->y *= (T)b; this->z *= (T)b; return *this;
	}; template<typename T> Vector3<T>& Pawn::Core::Math::Vector3<T>::operator*= (const int64& b) {
		this->x *= (T)b; this->y *= (T)b; this->z *= (T)b; return *this;
	}; template<typename T> Vector3<T>& Pawn::Core::Math::Vector3<T>::operator*= (const float32& b) {
		this->x *= (T)b; this->y *= (T)b; this->z *= (T)b; return *this;
	}; template<typename T> Vector3<T>& Pawn::Core::Math::Vector3<T>::operator*= (const float64& b) {
		this->x *= (T)b; this->y *= (T)b; this->z *= (T)b; return *this;
	};;
	template<typename T> Vector3<T>& Pawn::Core::Math::Vector3<T>::operator/= (const Vector3& b) {
		this->x /= (T)b.x; this->y /= (T)b.y; this->z /= (T)b.z; return *this;
	}; template<typename T> Vector3<T>& Pawn::Core::Math::Vector3<T>::operator/= (const int8& b) {
		this->x /= (T)b; this->y /= (T)b; this->z /= (T)b; return *this;
	}; template<typename T> Vector3<T>& Pawn::Core::Math::Vector3<T>::operator/= (const int16& b) {
		this->x /= (T)b; this->y /= (T)b; this->z /= (T)b; return *this;
	}; template<typename T> Vector3<T>& Pawn::Core::Math::Vector3<T>::operator/= (const int32& b) {
		this->x /= (T)b; this->y /= (T)b; this->z /= (T)b; return *this;
	}; template<typename T> Vector3<T>& Pawn::Core::Math::Vector3<T>::operator/= (const int64& b) {
		this->x /= (T)b; this->y /= (T)b; this->z /= (T)b; return *this;
	}; template<typename T> Vector3<T>& Pawn::Core::Math::Vector3<T>::operator/= (const float32& b) {
		this->x /= (T)b; this->y /= (T)b; this->z /= (T)b; return *this;
	}; template<typename T> Vector3<T>& Pawn::Core::Math::Vector3<T>::operator/= (const float64& b) {
		this->x /= (T)b; this->y /= (T)b; this->z /= (T)b; return *this;
	};;

	template<typename T>
	Vector3<T> Pawn::Core::Math::Vector3<T>::Normalize() const
	{
		T len = Length();
		return (*this) / len;
	}

	template<typename T>
	Vector3<T> Pawn::Core::Math::Vector3<T>::Lerp(Vector3& vec, float32 t) const
	{
		return LERP((*this), vec, t);
	}

	template<typename T>
	float64 Pawn::Core::Math::Vector3<T>::GetAngleBetweenVectors(const Vector3& other) const
	{
		return acos(this->Dot(other) / (this->Length() * other.Length()));
	}

	template<typename T>
	float64 Pawn::Core::Math::Vector3<T>::GetAngleBetweenVectorsInDegrees(const Vector3& other) const
	{
		return acos(this->Dot(other) / (this->Length() * other.Length()));
	}

	template<typename T>
	float64 Pawn::Core::Math::Vector3<T>::Dot(const Vector3& other) const
	{
		return (float64)this->x * (float64)other.x + (float64)this->y * (float64)other.y;
	}

	template<typename T>
	float64 Pawn::Core::Math::Vector3<T>::Cross(const Vector3& other) const
	{
		return (float64)this->x * (float64)other.x - (float64)this->y * (float64)other.y;
	}

	template<typename T>
	float64 Pawn::Core::Math::Vector3<T>::Length() const
	{
		return sqrt(SQUARE_SUM_2((float64)this->x, (float64)this->y));
	}

	template<typename T>
	float64 Pawn::Core::Math::Vector3<T>::LengthSquared() const
	{
		return SQUARE_SUM_2((float64)this->x, (float64)this->y);
	}
};

#undef _VECTOR3_OPERATOR_IMPL
#undef _VECTOR3_VEC_OPERATOR_IMPL
#undef _VECTOR3_OPERATOR_IMPL_EQUAL
#undef _VECTOR3_VEC_OPERATOR_IMPL_EQUAL
#undef VECTOR3_OPERATOR_IMPL
#undef VECTOR3_OPERATOR_IMPL_EQ
#undef _VECTOR3_OPERATOR_DEF
#undef _VECTOR3_OPERATOR_DEF_EQUAL
#undef VECTOR3_OPERATOR_DEF
#undef VECTOR3_OPERATOR_DEF_EQ