#pragma once

#include "Math.hpp"

template<typename T>
struct Vector2
{
	union
	{
		struct { T x, y; };
		T arr[2];
	};
	
	static constexpr int NumElements = 2;
	using ElemType = T;

	Vector2()         noexcept  : x(0), y(0) { }
	constexpr Vector2(T s)      noexcept  : x(s), y(s) { }
	constexpr Vector2(T a, T b) noexcept  : x(a), y(b) { }
	
	float Length()		  const { return sqrtf(LengthSquared()); }
	float LengthSquared() const { return x * x + y * y; }

	T& operator[] (int index) { return arr[index]; }
	T operator[] (int index) const { return arr[index]; }

	static float Distance(const Vector2<T> a, const Vector2<T> b) {
		float diffx = a.x - b.x;
		float diffy = a.y - b.y;
		return sqrtf(diffx * diffx + diffy * diffy);
	}

	static float DistanceSq(const Vector2<T> a, const Vector2<T> b) {
		float diffx = a.x - b.x;
		float diffy = a.y - b.y;
		return diffx * diffx + diffy * diffy;
	}
	//        mid  *
	// start  *     end  *
    static inline Vector2 CurveMove(const Vector2& pos, const Vector2& start, 
		const Vector2& mid, const Vector2& end, float t) noexcept
    {
      float t2 = t * t, t3 = t2 * t;
      return pos * (-t3 + 3.0f * t2 - 3.0f * t + 1.0f) +
           start * ( 3.0f * t3 - 6.0f * t2 + 3.0f * t) +
             mid * (-3.0f * t3 + 3.0f * t2) + end * t3;
    }
    // only float or double T
    static inline Vector2 Rotate(Vector2 v, float rad)
    {
      float st = sinf(rad), ct = cosf(rad);
      return Vector2((ct * v.x) - (st * v.y), (st * v.x) + (ct * v.y));
    }
  
    void Normalized() const { *this /= Length(); }
	
	Vector2 Normalize(Vector2 other) { return other.Normalize(); }
	
	Vector2 operator-() { return Vector2(-x, -y); }
	Vector2 operator+(Vector2 other) const { return Vector2(x + other.x, y + other.y); }
	Vector2 operator*(Vector2 other) const { return Vector2(x * other.x, y * other.y); }
	Vector2 operator/(Vector2 other) const { return Vector2(x / other.x, y / other.y); }
	Vector2 operator-(Vector2 other) const { return Vector2(x - other.x, y - other.y); }

	Vector2 operator+(T other) const { return Vector2(x + other, y + other); }
	Vector2 operator*(T other) const { return Vector2(x * other, y * other); }
	Vector2 operator/(T other) const { return Vector2(x / other, y / other); }
	Vector2 operator-(T other) const { return Vector2(x - other, y - other); }
	
	Vector2 operator += (Vector2 o) { x += o.x; y += o.y; return *this; }
	Vector2 operator *= (Vector2 o) { x *= o.x; y *= o.y; return *this; }
	Vector2 operator /= (Vector2 o) { x /= o.x; y /= o.y; return *this; }
	Vector2 operator -= (Vector2 o) { x -= o.x; y -= o.y; return *this; }

	Vector2 operator += (T o) { x += o; y += o; return *this; }
	Vector2 operator *= (T o) { x *= o; y *= o; return *this; }
	Vector2 operator /= (T o) { x /= o; y /= o; return *this; }
	Vector2 operator -= (T o) { x -= o; y -= o; return *this; }

	bool operator == (Vector2 b) const { return x == b.x && y == b.y; }
	bool operator != (Vector2 b) const { return x != b.x || y != b.y; }
	bool operator <  (Vector2 b) const { return x < b.x && y < b.y; }

	static constexpr Vector2 Zero()     { return Vector2( 0,  0); } 
	static constexpr Vector2 One()      { return Vector2( 1,  1); } 
	static constexpr Vector2 Up()       { return Vector2( 0,  1); } 
	static constexpr Vector2 Left()     { return Vector2(-1,  0); } 
	static constexpr Vector2 Down()     { return Vector2( 0, -1); } 
	static constexpr Vector2 Right()    { return Vector2( 1,  0); } 
};

template<typename T>
struct Vector3
{
	union
	{
		struct { T x, y, z; };
		T arr[3];
	};

	static constexpr int NumElements = 3;
	using ElemType = T;

	Vector3() : x(0), y(0), z(0) { }
	constexpr Vector3(T scale) : x(scale), y(scale), z(scale) { }
	constexpr Vector3(T a, T b, T c) : x(a), y(b), z(c) { }

	T& operator[] (int index) { return arr[index]; }
	T operator[] (int index) const { return arr[index]; }

	float Length() const { return sqrtf(LengthSquared()); }
	constexpr float LengthSquared() const { return x * x + y * y + z * z; }

	Vector3& Normalized() { *this /= Length(); return *this; }
	void NormalizeSelf() { *this /= Length(); }

	static float Distance(const Vector3& a, const Vector3& b)
	{
		Vector3 diff = a - b; diff *= diff;
		return sqrtf(diff.x + diff.y + diff.z);
	}

	// distance squared for comparing distances, faster than distance 
	static float DistanceSq(const Vector3& a, const Vector3& b)
	{
		Vector3 diff = a - b; diff *= diff;
		return diff.x + diff.y + diff.z;
	}

	static float Length(const Vector3& vec) { return vec.Length(); }

    static float Dot(const Vector3& a, const Vector3& b) noexcept
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	static Vector3 Lerp(const Vector3& a, const Vector3& b, float t)
	{
		return Vector3(
			a.x + (b.x - a.x) * t,
			a.y + (b.y - a.y) * t,
			a.z + (b.z - a.z) * t
		);
	}

	static Vector3 Cross(const Vector3& a, const Vector3& b)
	{
		return Vector3(a.y * b.z - b.y * a.z,
			a.z * b.x - b.z * a.x,
			a.x * b.y - b.x * a.y);
	}
	//        mid  *
	// start  *     end  * 
	static inline Vector3 CurveMove(const Vector3& pos, const Vector3& start, 
		const Vector3& mid, const Vector3& end, float t) noexcept
    {
      float t2 = t * t, t3 = t2 * t;
      return pos * (-t3 + 3.0f * t2 - 3.0f * t + 1.0f) +
           start * ( 3.0f * t3 - 6.0f * t2 + 3.0f * t) +
             mid * (-3.0f * t3 + 3.0f * t2) + end * t3;
    }

	static Vector3 Reflect(const Vector3& in, const Vector3& normal)
	{
		return in - normal * Dot(normal, in) * 2.0f;
	}
    static Vector3 Normalize(const Vector3& a) {
        return a / sqrtf(Dot(a, a));
    }
    // faster than Normalize but less accurate
	static Vector3 NormalizeEst(const Vector3& a) {
		return a * Math::RSqrt(Dot(a, a));
	}

	Vector3 operator-() { return Vector3(-x, -y, -z); }
	Vector3 operator+(const Vector3& other) const { return Vector3(x + other.x, y + other.y, z + other.z); }
	Vector3 operator*(const Vector3& other) const { return Vector3(x * other.x, y * other.y, z * other.z); }
	Vector3 operator/(const Vector3& other) const { return Vector3(x / other.x, y / other.y, z / other.z); }
	Vector3 operator-(const Vector3& other) const { return Vector3(x - other.x, y - other.y, z - other.z); }

	Vector3 operator+(T other) const { return Vector3(x + other, y + other, z + other); }
	Vector3 operator*(T other) const { return Vector3(x * other, y * other, z * other); }
	Vector3 operator/(T other) const { return Vector3(x / other, y / other, z / other); }
	Vector3 operator-(T other) const { return Vector3(x - other, y - other, z - other); }

	Vector3 operator += (const Vector3& o) { x += o.x; y += o.y; z += o.z; return *this; }
	Vector3 operator *= (const Vector3& o) { x *= o.x; y *= o.y; z *= o.z; return *this; }
	Vector3 operator /= (const Vector3& o) { x /= o.x; y /= o.y; z /= o.z; return *this; }
	Vector3 operator -= (const Vector3& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }

	Vector3 operator += (T o) { x += o; y += o; z += o; return *this; }
	Vector3 operator *= (T o) { x *= o; y *= o; z *= o; return *this; }
	Vector3 operator /= (T o) { x /= o; y /= o; z /= o; return *this; }
	Vector3 operator -= (T o) { x -= o; y -= o; z -= o; return *this; }

	Vector3 xxx() const { return Vector3(x, x, x); }
	Vector3 yyy() const { return Vector3(y, y, y); }
	Vector3 zzz() const { return Vector3(z, z, z); }

	static constexpr Vector3 Zero()    { return Vector3(0.0, 0.0, 0.0); }
	static constexpr Vector3 One()     { return Vector3(1.0, 1.0, 1.0); }
	static constexpr Vector3 Up()      { return Vector3(0.0, 1.0, 0.0); }
	static constexpr Vector3 Left()    { return Vector3(-1.0, 0.0, 0.0); }
	static constexpr Vector3 Down()    { return Vector3(0.0, -1.0, 0.0); }
	static constexpr Vector3 Right()   { return Vector3(1.0, 0.0, 0.0); }
	static constexpr Vector3 Forward() { return Vector3(0.0, 0.0, 1.0); }
	static constexpr Vector3 Backward(){ return Vector3(0.0, 0.0, -1.0); }
};

using Vector2d = Vector2<double>;
using Vector2f = Vector2<float>;
using Vector2i = Vector2<int>;
using Vector2s = Vector2<short>;
using Vector2c = Vector2<char>;

using Vector3d = Vector3<double>;
using Vector3f = Vector3<float>;
using Vector3i = Vector3<int>;
using Vector3s = Vector3<short>;
using Vector3c = Vector3<char>;

typedef Vector3f float3;
typedef Vector2f float2;

namespace Math
{
	template<typename T>
	FINLINE Vector3<T> Min(const Vector3<T>& a, const Vector3<T>& b) { return Vector3<T>(Min(a.x, b.x), Min(a.y, b.y), Min(a.z, b.z)); }
	template<typename T>
	FINLINE Vector3<T> Max(const Vector3<T>& a, const Vector3<T>& b) { return Vector3<T>(Max(a.x, b.x), Max(a.y, b.y), Max(a.z, b.z)); }

	template<typename T>
	FINLINE Vector2<T> Min(const Vector2<T>& a, const Vector2<T>& b) { return Vector2<T>(Min(a.x, b.x), Min(a.y, b.y)); }
	template<typename T>
	FINLINE Vector2<T> Max(const Vector2<T>& a, const Vector2<T>& b) { return Vector2<T>(Max(a.x, b.x), Max(a.y, b.y)); }

    template<typename T>
	FINLINE T Max3(const Vector3<T>& a) { return Max(Max(a.x, a.y), a.z); }
	template<typename T>
	FINLINE T Min3(const Vector3<T>& a) { return Min(Min(a.x, a.y), a.z); }

	FINLINE Vector2f ToVector2f(const Vector2i& vec) { return Vector2f((float)vec.x, (float)vec.y); }
	FINLINE Vector2i ToVector2f(const Vector2f& vec) { return Vector2i((int)vec.x, (int)vec.y); }

	constexpr FINLINE uint32 WangHash(uint32 s) {
		s = (s ^ 61u) ^ (s >> 16u);
		s *= 9, s = s ^ (s >> 4u);
		s *= 0x27d4eb2du;
		s = s ^ (s >> 15u);
		return s;
	}

	constexpr FINLINE uint64 MurmurHash(uint64 h) {
		h ^= h >> 33ul;
		h *= 0xff51afd7ed558ccdUL;
		h ^= h >> 33ul;
		h *= 0xc4ceb9fe1a85ec53UL;
		h ^= h >> 33ul;
		return h;
	}

	constexpr FINLINE uint64 VecToHash(Vector2c vec) 
    { 
		return uint64(vec.x * 3 ^ vec.y) | (uint64(vec.y) << 8ull); 
	}
	
	constexpr FINLINE uint64 VecToHash(Vector2s vec) 
    { 
		return (uint64)WangHash(uint64(vec.x) | (uint64(vec.y) << 16ull)); 
	}

	constexpr FINLINE uint64 VecToHash(Vector2i vec) 
    {
		return MurmurHash(uint64(vec.x) | (uint64(vec.y) << 32ull));
	}

	constexpr FINLINE uint64 VecToHash(Vector3c vec) 
	{
		return (uint64)WangHash(uint64(vec.x) | (uint64(vec.y) << 8ull) | (uint64(vec.z) << 16ull)); 
	}

	FINLINE uint64 VecToHash(Vector3s vec)
	{
		uint64 hash = vec.x;
		hash = (hash * 397ull) ^ vec.y;
		hash = (hash * 397ull) ^ vec.z;
		hash += hash << 3ull;
		hash ^= hash >> 11ull;
		hash += hash << 15ull;
		return hash;
	}

	FINLINE uint64 VecToHash(const Vector3i& vec)
	{
		uint64 hash = vec.x;
		hash = (hash * 397ull) ^ vec.y;
		hash = (hash * 397ull) ^ vec.z;
		hash += hash << 3ull;
		hash ^= hash >> 11ull;
		hash += hash << 15ull;
		return hash;
	}
	// for using these hashes with stl containers
	// namespace std {
	// 	template <> struct hash<Vector2s> {
	// 		uint64 operator()(const Vector2s& vec) const {
	// 			return VecToHash(vec);
	// 		}
	// 	};
	// }	
}