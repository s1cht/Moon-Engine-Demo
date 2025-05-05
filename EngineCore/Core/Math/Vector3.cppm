module;
export module Pawn.Core.Math.Vector3;

#include "Core.h"
#include "MathMacros.h"
import Pawn.Core;
import Pawn.Core.Math.Vector2;

export namespace Pawn::Core::Math
{
	template<typename T>
	struct Vector2;

	template<typename T>
	struct Vector3
	{
		static_assert(std::is_floating_point_v<T>, "T must be a number with floating point!");

	public:

		union
		{
			struct { T x, y, z; };
			struct { T X, Y, Z; };
			T XYZ[3];
		};

	public:
		CORE_API inline static const Vector3<T> ZeroVector() { static const Vector3<T> vec = Vector3<T>(0); return vec; }

		CORE_API inline static const Vector3<T> UpVector() { static const Vector3<T> vec = Vector3<T>(0, 1, 0); return vec; }

		CORE_API inline static const Vector3<T> DownVector() { static const Vector3<T> vec = Vector3<T>(0, -1, 0); return vec; }

		CORE_API inline static const Vector3<T> ForwardVector() { static const Vector3<T> vec = Vector3<T>(0, 0, 1); return vec; }

		CORE_API inline static const Vector3<T> BackwardVector() { static const Vector3<T> vec = Vector3<T>(0, 0, -1); return vec; }

		CORE_API inline static const Vector3<T> RightVector() { static const Vector3<T> vec = Vector3<T>(-1, 0, 0); return vec; }

		CORE_API inline static const Vector3<T> LeftVector() { static const Vector3<T> vec = Vector3<T>(1, 0, 0); return vec; }

	public:
		inline Vector3();

		inline Vector3(T scalar);

		inline Vector3(T _x, T _y, T _z);

		inline Vector3(const Vector2<T>& vec, T _z);

		inline Vector3(const Vector3& otherVec);

		inline Vector3(Vector3&& otherVec) noexcept;

	public:
		Vector3<T>& operator=(const Vector3& b);

		bool operator==(const Vector3& b) const;

		Vector3<T> operator+(const Vector3& b) const;

		Vector3<T> operator-(const Vector3& b) const;

		Vector3<T> operator*(const Vector3& b) const;

		template<typename U>
		Vector3<T> operator*(U scalar) const;

		Vector3<T> operator/(const Vector3& b) const;

		template<typename U>
		Vector3<T> operator/(U scalar) const;

		Vector3<T>& operator+=(const Vector3& b);

		Vector3<T>& operator-=(const Vector3& b);

		Vector3<T>& operator*=(const Vector3& b);

		template<typename U>
		Vector3<T>& operator*=(U scalar);

		Vector3<T>& operator/=(const Vector3& b);

		template<typename U>
		Vector3<T>& operator/=(U scalar);

		template<typename U>
		operator Vector3<U>() const;

	public:

		Vector3<T>& Normalize();

		Vector3<T> Normalized() const;

		Vector3<T> Lerp(const Vector3& vec, float32 t) const;

		float64 GetAngleBetweenVectors(const Vector3& other) const;

		float64 GetAngleBetweenVectorsInDegrees(const Vector3& other) const;

		T Dot(const Vector3& other) const;

		Vector3<T> Cross(const Vector3& other) const;

		T Length() const;

		T LengthSquared() const;

		Vector3<T> Project(const Vector3& other) const;

		Vector3<T> Reflect(const Vector3& normal) const;

	};

	template<typename T>
	inline Vector3<T>::Vector3() : x(0), y(0), z(0) {}

	template<typename T>
	inline Vector3<T>::Vector3(T scalar) : x(scalar), y(scalar), z(scalar) {}

	template<typename T>
	inline Vector3<T>::Vector3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}

	template<typename T>
	inline Vector3<T>::Vector3(const Vector2<T>& vec, T _z) : x(vec.x), y(vec.y), z(_z) {}

	template<typename T>
	inline Vector3<T>::Vector3(const Vector3& otherVec) : x(otherVec.x), y(otherVec.y), z(otherVec.z) {}

	template<typename T>
	inline Vector3<T>::Vector3(Vector3&& otherVec) noexcept : x(otherVec.x), y(otherVec.y), z(otherVec.z) {}

	template<typename T>
	inline Vector3<T>& Vector3<T>::operator=(const Vector3& b)
	{
		x = b.x;
		y = b.y;
		z = b.z;
		return *this;
	}

	template<typename T>
	inline bool Vector3<T>::operator==(const Vector3& b) const
	{
		if constexpr (std::is_floating_point_v<T>)
		{
			constexpr T epsilon = static_cast<T>(1e-6);
			return std::abs(x - b.x) < epsilon &&
				std::abs(y - b.y) < epsilon &&
				std::abs(z - b.z) < epsilon;
		}
		else
		{
			return x == b.x && y == b.y && z == b.z;
		}
	}

	template<typename T>
	inline Vector3<T> Vector3<T>::operator+(const Vector3& b) const
	{
		return Vector3<T>(x + b.x, y + b.y, z + b.z);
	}

	template<typename T>
	inline Vector3<T> Vector3<T>::operator-(const Vector3& b) const
	{
		return Vector3<T>(x - b.x, y - b.y, z - b.z);
	}

	template<typename T>
	inline Vector3<T> Vector3<T>::operator*(const Vector3& b) const
	{
		return Vector3<T>(x * b.x, y * b.y, z * b.z);
	}

	template<typename T>
	template<typename U>
	inline Vector3<T> Vector3<T>::operator*(U scalar) const
	{
		return Vector3<T>(static_cast<T>(x * scalar), static_cast<T>(y * scalar), static_cast<T>(z * scalar));
	}

	template<typename T>
	inline Vector3<T> Vector3<T>::operator/(const Vector3& b) const
	{
		return Vector3<T>(
			b.x != 0 ? x / b.x : 0,
			b.y != 0 ? y / b.y : 0,
			b.z != 0 ? z / b.z : 0
		);
	}

	template<typename T>
	template<typename U>
	inline Vector3<T> Vector3<T>::operator/(U scalar) const
	{
		if (scalar == 0) return Vector3<T>(0);
		return Vector3<T>(static_cast<T>(x / scalar), static_cast<T>(y / scalar), static_cast<T>(z / scalar));
	}

	template<typename T>
	inline Vector3<T>& Vector3<T>::operator+=(const Vector3& b)
	{
		x += b.x;
		y += b.y;
		z += b.z;
		return *this;
	}

	template<typename T>
	inline Vector3<T>& Vector3<T>::operator-=(const Vector3& b)
	{
		x -= b.x;
		y -= b.y;
		z -= b.z;
		return *this;
	}

	template<typename T>
	inline Vector3<T>& Vector3<T>::operator*=(const Vector3& b)
	{
		x *= b.x;
		y *= b.y;
		z *= b.z;
		return *this;
	}

	template<typename T>
	template<typename U>
	inline Vector3<T>& Vector3<T>::operator*=(U scalar)
	{
		x = static_cast<T>(x * scalar);
		y = static_cast<T>(y * scalar);
		z = static_cast<T>(z * scalar);
		return *this;
	}

	template<typename T>
	inline Vector3<T>& Vector3<T>::operator/=(const Vector3& b)
	{
		x = b.x != 0 ? x / b.x : 0;
		y = b.y != 0 ? y / b.y : 0;
		z = b.z != 0 ? z / b.z : 0;
		return *this;
	}

	template<typename T>
	template<typename U>
	inline Vector3<T>& Vector3<T>::operator/=(U scalar)
	{
		if (scalar == 0) return *this = Vector3<T>(0);
		x = static_cast<T>(x / scalar);
		y = static_cast<T>(y / scalar);
		z = static_cast<T>(z / scalar);
		return *this;
	}

	template<typename T>
	template<typename U>
	inline Vector3<T>::operator Vector3<U>() const
	{
		return Vector3<U>(static_cast<U>(x), static_cast<U>(y), static_cast<U>(z));
	}

	template<typename T>
	inline Vector3<T>& Vector3<T>::Normalize()
	{
		T len = Length();
		if (len > 0.0)
		{
			x /= len;
			y /= len;
			z /= len;
		}
		return *this;
	}

	template<typename T>
	inline Vector3<T> Vector3<T>::Normalized() const
	{
		T len = Length();
		if (len > 0.0)
			return Vector3<T>(x / len, y / len, z / len);
		return *this;
	}

	template<typename T>
	inline Vector3<T> Vector3<T>::Lerp(const Vector3& vec, float32 t) const
	{
		return LERP(*this, vec, t);
	}

	template<typename T>
	inline float64 Vector3<T>::GetAngleBetweenVectors(const Vector3& other) const
	{
		T dot = Dot(other);
		T len = Length() * other.Length();
		if (len == 0) return 0.0;
		return std::acos(static_cast<float64>(dot) / static_cast<float64>(len));
	}

	template<typename T>
	inline float64 Vector3<T>::GetAngleBetweenVectorsInDegrees(const Vector3& other) const
	{
		return GetAngleBetweenVectors(other) * 180.0 / PI;
	}

	template<typename T>
	inline T Vector3<T> ::Dot(const Vector3& other) const
	{
		return x * other.x + y * other.y + z * other.z;
	}

	template<typename T>
	inline Vector3<T> Vector3<T>::Cross(const Vector3& other) const
	{
		return Vector3<T>(
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x
		);
	}

	template<typename T>
	inline T Vector3<T>::Length() const
	{
		return static_cast<T>(sqrt(SQUARE_SUM_3(x, y, z)));
	}

	template<typename T>
	inline T Vector3<T>::LengthSquared() const
	{
		return SQUARE_SUM_3(x, y, z);
	}

	template<typename T>
	inline Vector3<T> Vector3<T>::Project(const Vector3& other) const
	{
		T dot = Dot(other);
		T lenSquared = other.LengthSquared();
		if (lenSquared == 0) return Vector3<T>(0);
		return other * (dot / lenSquared);
	}

	template<typename T>
	inline Vector3<T> Vector3<T>::Reflect(const Vector3& normal) const
	{
		return *this - normal * (2 * Dot(normal));
	}

	template<typename T, typename U>
	inline Vector3<T> operator*(U scalar, const Vector3<T>& vec)
	{
		return vec * scalar;
	}

}