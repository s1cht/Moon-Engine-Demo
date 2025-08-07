#pragma once

#include "Core.hpp"
#include "Core/Math/MathMacros.hpp"
#include "Core/Utils/Logging/Logger.hpp"

namespace ME::Core::Math
{
	template<typename T>
	struct Vector2
	{
		static_assert(std::is_floating_point_v<T>, "T must be a number with floating point!");

	public:
		union
		{
			struct { T x, y; };
			struct { T X, Y; };
			T XY[2];
		};

	public:
		CORE_API static const Vector2<T> ZeroVector;

		CORE_API static const Vector2<T> UpVector;

		CORE_API static const Vector2<T> DownVector;

		CORE_API static const Vector2<T> LeftVector;

		CORE_API static const Vector2<T> RightVector;

	public:
		inline Vector2();

		inline Vector2(T scalar);

		inline Vector2(T _x, T _y);

		inline Vector2(const Vector2& otherVec);

		inline Vector2(Vector2&& otherVec) noexcept;

	public:
		Vector2<T>& operator=(const Vector2& b);

		bool operator==(const Vector2& b) const;

		Vector2<T> operator+(const Vector2& b) const;

		Vector2<T> operator-(const Vector2& b) const;

		Vector2<T> operator*(const Vector2& b) const;

		template<typename U>
		Vector2<T> operator*(U scalar) const;

		Vector2<T> operator/(const Vector2& b) const;

		template<typename U>
		Vector2<T> operator/(U scalar) const;

		Vector2<T>& operator+=(const Vector2& b);

		Vector2<T>& operator-=(const Vector2& b);

		Vector2<T>& operator*=(const Vector2& b);

		template<typename U>
		Vector2<T>& operator*=(U scalar);

		Vector2<T>& operator/=(const Vector2& b);

		template<typename U>
		Vector2<T>& operator/=(U scalar);

	public:

		Vector2<T> Normalize() const;

		Vector2<T> Lerp(const Vector2& vec, float32 t) const;

		float64 GetAngleBetweenVectors(const Vector2& other) const;

		float64 GetAngleBetweenVectorsInDegrees(const Vector2& other) const;

		T Dot(const Vector2& other) const;

		T Cross(const Vector2& other) const;

		T Length() const;

		T LengthSquared() const;

		Vector2<T> Project(const Vector2& other) const;

		Vector2<T> Reflect(const Vector2& normal) const;

	};

	template<typename T>
	inline Vector2<T>::Vector2() : x(0), y(0) {}

	template<typename T>
	inline Vector2<T>::Vector2(T scalar) : x(scalar), y(scalar) {}

	template<typename T>
	inline Vector2<T>::Vector2(T _x, T _y) : x(_x), y(_y) {}

	template<typename T>
	inline Vector2<T>::Vector2(const Vector2& otherVec) : x(otherVec.x), y(otherVec.y) {}

	template<typename T>
	inline Vector2<T>::Vector2(Vector2&& otherVec) noexcept : x(otherVec.x), y(otherVec.y) {}

	template<typename T>
	inline Vector2<T>& Vector2<T>::operator=(const Vector2& b)
	{
		x = b.x;
		y = b.y;
		return *this;
	}

	template<typename T>
	inline bool Vector2<T>::operator==(const Vector2& b) const
	{
		if constexpr (std::is_floating_point_v<T>)
		{
			constexpr T epsilon = static_cast<T>(1e-6);
			return std::abs(x - b.x) < epsilon &&
				std::abs(y - b.y) < epsilon;
		}
		else
		{
			return x == b.x && y == b.y;
		}
	}

	template<typename T>
	inline Vector2<T> Vector2<T>::operator+(const Vector2& b) const
	{
		return Vector2<T>(x + b.x, y + b.y);
	}

	template<typename T>
	inline Vector2<T> Vector2<T>::operator-(const Vector2& b) const
	{
		return Vector2<T>(x - b.x, y - b.y);
	}

	template<typename T>
	inline Vector2<T> Vector2<T>::operator*(const Vector2& b) const
	{
		return Vector2<T>(x * b.x, y * b.y);
	}

	template<typename T>
	template<typename U>
	inline Vector2<T> Vector2<T>::operator*(U scalar) const
	{
		return Vector2<T>(static_cast<T>(x * scalar), static_cast<T>(y * scalar));
	}

	template<typename T>
	inline Vector2<T> Vector2<T>::operator/(const Vector2& b) const
	{
		if (b == Vector2::ZeroVector) ME_ASSERT(false, TEXT("Multiplication by zero"));
		return Vector2<T>(
			b.x != 0 ? x / b.x : 0,
			b.y != 0 ? y / b.y : 0
		);
	}

	template<typename T>
	template<typename U>
	inline Vector2<T> Vector2<T>::operator/(U scalar) const
	{
		if (scalar == 0) return Vector2<T>(0);
		return Vector2<T>(static_cast<T>(x / scalar), static_cast<T>(y / scalar));
	}

	template<typename T>
	inline Vector2<T>& Vector2<T>::operator+=(const Vector2& b)
	{
		x += b.x;
		y += b.y;
		return *this;
	}

	template<typename T>
	inline Vector2<T>& Vector2<T>::operator-=(const Vector2& b)
	{
		x -= b.x;
		y -= b.y;
		return *this;
	}

	template<typename T>
	inline Vector2<T>& Vector2<T>::operator*=(const Vector2& b)
	{
		x *= b.x;
		y *= b.y;
		return *this;
	}

	template<typename T>
	template<typename U>
	inline Vector2<T>& Vector2<T>::operator*=(U scalar)
	{
		x = static_cast<T>(x * scalar);
		y = static_cast<T>(y * scalar);
		return *this;
	}

	template<typename T>
	inline Vector2<T>& Vector2<T>::operator/=(const Vector2& b)
	{
		x = b.x != 0 ? x / b.x : 0;
		y = b.y != 0 ? y / b.y : 0;
		return *this;
	}

	template<typename T>
	template<typename U>
	inline Vector2<T>& Vector2<T>::operator/=(U scalar)
	{
		if (scalar == 0) return *this = Vector2<T>(0);
		x = static_cast<T>(x / scalar);
		y = static_cast<T>(y / scalar);
		return *this;
	}

	template<typename T>
	inline Vector2<T> Vector2<T>::Normalize() const
	{
		T len = Length();
		if (len == 0) return Vector2<T>(0);
		return *this / len;
	}

	template<typename T>
	inline Vector2<T> Vector2<T>::Lerp(const Vector2& vec, float32 t) const
	{
		return LERP(*this, vec, t);
	}

	template<typename T>
	inline float64 Vector2<T>::GetAngleBetweenVectors(const Vector2& other) const
	{
		T dot = Dot(other);
		T len = Length() * other.Length();
		if (len == 0) return 0.0;
		return std::acos(static_cast<float64>(dot) / static_cast<float64>(len));
	}

	template<typename T>
	inline float64 Vector2<T>::GetAngleBetweenVectorsInDegrees(const Vector2& other) const
	{
		return GetAngleBetweenVectors(other) * 180.0 / PI;
	}

	template<typename T>
	inline T Vector2<T>::Dot(const Vector2& other) const
	{
		return x * other.x + y * other.y;
	}

	template<typename T>
	inline T Vector2<T>::Cross(const Vector2& other) const
	{
		return x * other.y - y * other.x;
	}

	template<typename T>
	inline T Vector2<T>::Length() const
	{
		return static_cast<T>(std::sqrt(static_cast<double>(x * x + y * y)));
	}

	template<typename T>
	inline T Vector2<T>::LengthSquared() const
	{
		return x * x + y * y;
	}

	template<typename T>
	inline Vector2<T> Vector2<T>::Project(const Vector2& other) const
	{
		T dot = Dot(other);
		T lenSquared = other.LengthSquared();
		if (lenSquared == 0) return Vector2<T>(0);
		return other * (dot / lenSquared);
	}

	template<typename T>
	inline Vector2<T> Vector2<T>::Reflect(const Vector2& normal) const
	{
		return *this - normal * (2 * Dot(normal));
	}

	template<typename T, typename U>
	inline Vector2<T> operator*(U scalar, const Vector2<T>& vec)
	{
		return vec * scalar;
	}

}