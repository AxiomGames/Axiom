#pragma once

#include "Axiom/Core/Common.hpp"
	
namespace ax
{

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

	[[nodiscard]] FINLINE float Length()		const noexcept { return sqrtf(LengthSquared()); }
	[[nodiscard]] FINLINE float LengthSquared() const noexcept { return x * x + y * y; }

	FINLINE static float Distance(const Vector2<T> a, const Vector2<T> b)
	{
		float diffx = a.x - b.x;
		float diffy = a.y - b.y;
		return sqrtf(diffx * diffx + diffy * diffy);
	}

	FINLINE void Normalize() const { *this /= Length(); }

	[[nodiscard]] FINLINE Vector2<T> operator + (Vector2<T> b) const noexcept { return Vector2<T>(x + b.x, y + b.y); }
	[[nodiscard]] FINLINE Vector2<T> operator - (Vector2<T> b) const noexcept { return Vector2<T>(x - b.x, y - b.y); }
	[[nodiscard]] FINLINE Vector2<T> operator * (Vector2<T> b) const noexcept { return Vector2<T>(x * b.x, y * b.y); }
	[[nodiscard]] FINLINE Vector2<T> operator / (Vector2<T> b) const noexcept { return Vector2<T>(x / b.x, y / b.y); }
	[[nodiscard]] FINLINE Vector2<T> operator += (Vector2<T> b) noexcept { x += b.x; y += b.y; return *this; }
	[[nodiscard]] FINLINE Vector2<T> operator -= (Vector2<T> b) noexcept { x -= b.x; y -= b.y; return *this; }
	[[nodiscard]] FINLINE Vector2<T> operator *= (Vector2<T> b) noexcept { x *= b.x; y *= b.y; return *this; }
	[[nodiscard]] FINLINE Vector2<T> operator /= (Vector2<T> b) noexcept { x /= b.x; y /= b.y; return *this; }
	[[nodiscard]] FINLINE Vector2<T> operator *  (T b) const noexcept    { return Vector2<T>(x * b, y * b); }
	[[nodiscard]] FINLINE Vector2<T> operator /  (T b) const noexcept    { return Vector2<T>(x / b, y / b); } 
	[[nodiscard]] FINLINE Vector2<T> operator *= (T b) noexcept { x *= b; y *= b; return *this; }
	[[nodiscard]] FINLINE Vector2<T> operator /= (T b) noexcept { x /= b; y /= b; return *this; }
};

typedef Vector2<double> Vector2d;
typedef Vector2<int> Vector2i;

}