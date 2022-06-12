#pragma once

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

		__forceinline Vector2() : x(0), y(0) {}
		__forceinline Vector2(T scale) : x(scale), y(scale) {}
		__forceinline Vector2(T _x, T _y) : x(_x), y(_y) {}

		[[nodiscard]] __forceinline float Length() const noexcept { return sqrtf(LengthSquared()); }
		[[nodiscard]] __forceinline float LengthSquared() const noexcept { return x * x + y * y; }


		__forceinline static float Distance(const Vector2<T> a, const Vector2<T> b)
		{
			float diffx = a.x - b.x;
			float diffy = a.y - b.y;
			return sqrtf(diffx * diffx + diffy * diffy);
		}

		void Normalize() const { *this /= Length(); }

		[[nodiscard]] __forceinline Vector2<T> operator + (Vector2<T> b) const noexcept { return Vector2<T>(x + b.x, y + b.y); }
		[[nodiscard]] __forceinline Vector2<T> operator - (Vector2<T> b) const noexcept { return Vector2<T>(x - b.x, y - b.y); }
		[[nodiscard]] __forceinline Vector2<T> operator * (Vector2<T> b) const noexcept { return Vector2<T>(x * b.x, y * b.y); }
		[[nodiscard]] __forceinline Vector2<T> operator / (Vector2<T> b) const noexcept { return Vector2<T>(x / b.x, y / b.y); }

		[[nodiscard]] __forceinline Vector2<T> operator += (Vector2<T> b) noexcept { x += b.x; y += b.y; return *this; }
		[[nodiscard]] __forceinline Vector2<T> operator -= (Vector2<T> b) noexcept { x -= b.x; y -= b.y; return *this; }
		[[nodiscard]] __forceinline Vector2<T> operator *= (Vector2<T> b) noexcept { x *= b.x; y *= b.y; return *this; }
		[[nodiscard]] __forceinline Vector2<T> operator /= (Vector2<T> b) noexcept { x /= b.x; y /= b.y; return *this; }

		[[nodiscard]] __forceinline Vector2<T> operator *  (T b) const noexcept { return Vector2<T>(x * b, y * b); }
		[[nodiscard]] __forceinline Vector2<T> operator /  (T b) const noexcept { return Vector2<T>(x / b, y / b); }
		[[nodiscard]] __forceinline Vector2<T> operator *= (T b) noexcept { x *= b; y *= b; return *this; }
		[[nodiscard]] __forceinline Vector2<T> operator /= (T b) noexcept { x /= b; y /= b; return *this; }
	};

	typedef Vector2<double>	Vector2d;
	typedef Vector2<int>	Vector2i;
}