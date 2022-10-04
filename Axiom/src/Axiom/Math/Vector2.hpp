#pragma once

#include "../Core/Common.hpp"

template<typename T = float>
struct Vector2
{
	union
	{
		struct { T x, y; };
		struct { T width, height; };
		float arr[2];
	};

	FINLINE Vector2()           noexcept : x(0)    , y(0)     {}
	FINLINE Vector2(T scale)    noexcept : x(scale), y(scale) {}
	FINLINE Vector2(T _x, T _y) noexcept : x(_x)   , y(_y)    {}

	FINLINE float Length()		const noexcept { return sqrtf(LengthSquared()); }
	FINLINE float LengthSquared() const noexcept { return x * x + y * y; }

	FINLINE static float Distance(const Vector2<T> a, const Vector2<T> b)
	{
		float diffx = a.x - b.x;
		float diffy = a.y - b.y;
		return sqrtf(diffx * diffx + diffy * diffy);
	}

	FINLINE void Normalize() const { *this /= Length(); }

	FINLINE Vector2<T> operator + (Vector2<T> b) const noexcept { return Vector2<T>(x + b.x, y + b.y); }
	FINLINE Vector2<T> operator - (Vector2<T> b) const noexcept { return Vector2<T>(x - b.x, y - b.y); }
	FINLINE Vector2<T> operator * (Vector2<T> b) const noexcept { return Vector2<T>(x * b.x, y * b.y); }
	FINLINE Vector2<T> operator / (Vector2<T> b) const noexcept { return Vector2<T>(x / b.x, y / b.y); }
	FINLINE Vector2<T> operator += (Vector2<T> b) noexcept { x += b.x; y += b.y; return *this; }
	FINLINE Vector2<T> operator -= (Vector2<T> b) noexcept { x -= b.x; y -= b.y; return *this; }
	FINLINE Vector2<T> operator *= (Vector2<T> b) noexcept { x *= b.x; y *= b.y; return *this; }
	FINLINE Vector2<T> operator /= (Vector2<T> b) noexcept { x /= b.x; y /= b.y; return *this; }
	FINLINE Vector2<T> operator *  (T b) const noexcept    { return Vector2<T>(x * b, y * b); }
	FINLINE Vector2<T> operator /  (T b) const noexcept    { return Vector2<T>(x / b, y / b); }
	FINLINE Vector2<T> operator *= (T b) noexcept { x *= b; y *= b; return *this; }
	FINLINE Vector2<T> operator /= (T b) noexcept { x /= b; y /= b; return *this; }
};

typedef Vector2<double> Vector2d;
typedef Vector2<float> Vector2f;
typedef Vector2<int> Vector2i;