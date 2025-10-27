#pragma once 

#include "Core.hpp"
#include "Core/Math/MathMacros.hpp"
#include "Core/Math/Vector2.hpp"
#include "Core/Math/Vector3.hpp"

namespace ME::Core::Math
{
	template<typename T>
	struct Vector2;

	template<typename T>
	struct Vector3;

	template<typename T>
	struct Vector4
	{
		static_assert(std::is_floating_point_v<T>, "T must be a number with floating point!");

	public:
		union
		{
			struct { T x, y, z, w; };
			struct { T X, Y, Z, W; };
			T XYZW[4];
		};

	public:
		COREAPI static const Vector4<T> ZeroVector;

		COREAPI static const Vector4<T> OneVector;

	public:
		Vector4();

		inline Vector4(T scalar);

		inline Vector4(T _x, T _y, T _z, T _w);

		inline Vector4(const Vector2<T>& xy, const Vector2<T>& xw);

		inline Vector4(const Vector3<T>& vec, T _w);

		inline Vector4(const Vector4& otherVec);

		inline Vector4(Vector4&& otherVec) noexcept;

	public:
		bool operator==(const Vector4& b) const;

		Vector4<T>& operator=(const Vector4& b);

		Vector4<T> operator+(const Vector4& b) const;

		Vector4<T> operator-(const Vector4& b) const;

		Vector4<T> operator*(const Vector4& b) const;

		Vector4<T> operator/(const Vector4& b) const;

		template<typename U>
		Vector4<T> operator*(U scalar) const;

		template<typename U>
		Vector4<T> operator/(U scalar) const;

		Vector4<T>& operator+=(const Vector4& b);

		Vector4<T>& operator-=(const Vector4& b);

		Vector4<T>& operator*=(const Vector4& b);

		Vector4<T>& operator/=(const Vector4& b);
		
		template<typename U>
		Vector4<T>& operator*=(U scalar);
		
		template<typename U>
		Vector4<T>& operator/=(U scalar);

	public:
		Vector4<T>& Normalize();

		Vector4<T> Normalized() const;
		
		Vector4<T> Lerp(const Vector4& vec, float32 t) const;

		Vector4<T> Cross(const Vector4& other) const;

		float64 GetAngleBetweenVectors(const Vector4& other) const;

		float64 GetAngleBetweenVectorsInDegrees(const Vector4& other) const;

		T Dot(const Vector4& other) const;

		T Length() const;

		T LengthSquared() const;

		Vector4<T> Project(const Vector4& other) const;

		Vector4<T> Reflect(const Vector4& normal) const;

	};

	template<typename T>
	Vector4<T>::Vector4() : x(0), y(0), z(0), w(0) {}

	template<typename T>
	Vector4<T>::Vector4(T scalar) : x(scalar), y(scalar), z(scalar), w(scalar) {}

	template<typename T>
	Vector4<T>::Vector4(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) {}

	template<typename T>
	Vector4<T>::Vector4(const Vector2<T>& xy, const Vector2<T>& zw)
		: x(xy.x), y(xy.y), z(zw.x), w(zw.y) {
	}

	template<typename T>
	Vector4<T>::Vector4(const Vector3<T>& vec, T _w)
		: x(vec.x), y(vec.y), z(vec.z), w(_w) {
	}

	template<typename T>
	Vector4<T>::Vector4(const Vector4& otherVec)
		: x(otherVec.x), y(otherVec.y), z(otherVec.z), w(otherVec.w) {
	}

	template<typename T>
	Vector4<T>::Vector4(Vector4&& otherVec) noexcept
		: x(otherVec.x), y(otherVec.y), z(otherVec.z), w(otherVec.w) {
	}

	template<typename T>
	inline Vector4<T>& Vector4<T>::operator=(const Vector4& b)
	{
		x = b.x;
		y = b.y;
		z = b.z;
		w = b.w;
		return *this;
	}

	template<typename T>
	inline bool Vector4<T>::operator==(const Vector4& b) const
	{
		if constexpr (std::is_floating_point_v<T>)
		{
			constexpr T epsilon = static_cast<T>(1e-6);
			return std::abs(x - b.x) < epsilon &&
				std::abs(y - b.y) < epsilon &&
				std::abs(z - b.z) < epsilon &&
				std::abs(w - b.w) < epsilon;
		}
		else
		{
			return x == b.x && y == b.y && z == b.z && w == b.w;
		}
	}

	template<typename T>
	inline Vector4<T> Vector4<T>::operator+(const Vector4& b) const
	{
		return Vector4<T>(x + b.x, y + b.y, z + b.z, w + b.w);
	}

	template<typename T>
	inline Vector4<T> Vector4<T>::operator-(const Vector4& b) const
	{
		return Vector4<T>(x - b.x, y - b.y, z - b.z, w - b.w);
	}

	template<typename T>
	inline Vector4<T> Vector4<T>::operator*(const Vector4& b) const
	{
		return Vector4<T>(x * b.x, y * b.y, z * b.z, w * b.w);
	}

	template<typename T>
	template<typename U>
	inline Vector4<T> Vector4<T>::operator*(U scalar) const
	{
		return Vector4<T>(static_cast<T>(x * scalar), static_cast<T>(y * scalar), static_cast<T>(z * scalar), static_cast<T>(w * scalar));
	}

	template<typename T>
	inline Vector4<T> Vector4<T>::operator/(const Vector4& b) const
	{
		return Vector4<T>(
			b.x != 0 ? x / b.x : 0,
			b.y != 0 ? y / b.y : 0,
			b.z != 0 ? z / b.z : 0,
			b.w != 0 ? w / b.w : 0
		);
	}

	template<typename T>
	template<typename U>
	inline Vector4<T> Vector4<T>::operator/(U scalar) const
	{
		if (scalar == 0) return Vector4<T>(0);
		return Vector4<T>(static_cast<T>(x / scalar), static_cast<T>(y / scalar), static_cast<T>(z / scalar), static_cast<T>(w / scalar));
	}

	template<typename T>
	inline Vector4<T>& Vector4<T>::operator+=(const Vector4& b)
	{
		x += b.x;
		y += b.y;
		z += b.z;
		w += b.w;
		return *this;
	}

	template<typename T>
	inline Vector4<T>& Vector4<T>::operator-=(const Vector4& b)
	{
		x -= b.x;
		y -= b.y;
		z -= b.z;
		w -= b.w;
		return *this;
	}

	template<typename T>
	inline Vector4<T>& Vector4<T>::operator*=(const Vector4& b)
	{
		x *= b.x;
		y *= b.y;
		z *= b.z;
		w *= b.w;
		return *this;
	}

	template<typename T>
	template<typename U>
	inline Vector4<T>& Vector4<T>::operator*=(U scalar)
	{
		x = static_cast<T>(x * scalar);
		y = static_cast<T>(y * scalar);
		z = static_cast<T>(z * scalar);
		w = static_cast<T>(w * scalar);
		return *this;
	}

	template<typename T>
	inline Vector4<T>& Vector4<T>::operator/=(const Vector4& b)
	{
		x = b.x != 0 ? x / b.x : 0;
		y = b.y != 0 ? y / b.y : 0;
		z = b.z != 0 ? z / b.z : 0;
		w = b.w != 0 ? w / b.w : 0;
		return *this;
	}

	template<typename T>
	template<typename U>
	inline Vector4<T>& Vector4<T>::operator/=(U scalar)
	{
		if (scalar == 0) return *this = Vector4<T>(0);
		x = static_cast<T>(x / scalar);
		y = static_cast<T>(y / scalar);
		z = static_cast<T>(z / scalar);
		w = static_cast<T>(w / scalar);
		return *this;
	}

	template<typename T>
	inline Vector4<T>& Vector4<T>::Normalize()
	{
		T len = Length();
		if constexpr (std::is_floating_point_v<T>)
		{
			if (len > static_cast<T>(0.0))
			{
				x /= len;
				y /= len;
				z /= len;
				w /= len;
			}
		}
		else
		{
			if (len != 0)
			{
				x /= len;
				y /= len;
				z /= len;
				w /= len;
			}
		}
		return *this;
	}

	template<typename T>
	inline Vector4<T> Vector4<T>::Normalized() const
	{
		T len = Length();
		if constexpr (std::is_floating_point_v<T>)
		{
			if (len > static_cast<T>(0.0))
			{
				return Vector4<T>(x / len, y / len, z / len, w / len);
			}
		}
		else
		{
			if (len != 0)
			{
				return Vector4<T>(x / len, y / len, z / len, w / len);
			}
		}
		return Vector4<T>(0);
	}

	template<typename T>
	inline Vector4<T> Vector4<T>::Lerp(const Vector4& vec, float32 t) const
	{
		return LERP(*this, vec, t);
	}

	template<typename T>
	inline Vector4<T> Vector4<T>::Cross(const Vector4& other) const
	{
		return Vector4<T>(
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x,
			0
		);
	}

	template<typename T>
	inline float64 Vector4<T>::GetAngleBetweenVectors(const Vector4& other) const
	{
		T dot = Dot(other);
		T len = Length() * other.Length();
		if (len == 0) return 0.0;
		return std::acos(static_cast<float64>(dot) / static_cast<float64>(len));
	}

	template<typename T>
	inline float64 Vector4<T>::GetAngleBetweenVectorsInDegrees(const Vector4& other) const
	{
		return GetAngleBetweenVectors(other) * 180.0 / PI;
	}

	template<typename T>
	inline T Vector4<T>::Dot(const Vector4& other) const
	{
		return x * other.x + y * other.y + z * other.z + w * other.w;
	}

	template<typename T>
	inline T Vector4<T>::Length() const
	{
		return static_cast<T>(std::sqrt(static_cast<double>(x * x + y * y + z * z + w * w)));
	}

	template<typename T>
	inline T Vector4<T>::LengthSquared() const
	{
		return x * x + y * y + z * z + w * w;
	}

	template<typename T>
	inline Vector4<T> Vector4<T>::Project(const Vector4& other) const
	{
		T dot = Dot(other);
		T lenSquared = other.LengthSquared();
		if (lenSquared == 0) return Vector4<T>(0);
		return other * (dot / lenSquared);
	}

	template<typename T>
	inline Vector4<T> Vector4<T>::Reflect(const Vector4& normal) const
	{
		return *this - normal * (2 * Dot(normal));
	}

	template<typename T, typename U>
	inline Vector4<T> operator*(U scalar, const Vector4<T>& vec)
	{
		return vec * scalar;
	}

}
