#pragma once

#include "Math.hpp"

AMATH_NAMESPACE

struct Vector3
{
	union
	{
		struct { float x, y, z; };
		float arr[3];
	};

	FINLINE Vector3() : x(0), y(0), z(0) {}
	FINLINE Vector3(float scale)		  noexcept : x(scale), y(scale), z(scale) {}
	FINLINE Vector3(float _x, float _y, float _z) noexcept : x(_x), y(_y), z(_z) {}
	VECTORCALL Vector3(__m128 _vec) { _mm_store_ps(arr, _vec); }

	FINLINE float Length() const { return sqrtf(LengthSquared()); }
	FINLINE float LengthSquared() const { return x * x + y * y + z * z; }
	
	inline __m128 vec() const
	{
		float tmp[4];
		tmp[0] = x;
		tmp[1] = y;
		tmp[2] = z;
		return _mm_load_ps(tmp);
	};

	FINLINE Vector3& Normalized() { *this /= Length(); return *this; }

	FINLINE static float VECTORCALL Distance(const Vector3 a, const Vector3 b)
	{
		Vector3 diff = a - b;
		diff *= diff;
		return sqrtf(diff.x + diff.y + diff.z);
	}

	FINLINE static float Length(const Vector3& vec) noexcept { return vec.Length(); }

	FINLINE static float Dot(const Vector3& a, const Vector3& b) noexcept
	{
		return a.arr[0] * b.arr[0] + a.arr[1] * b.arr[1] + a.arr[2] * b.arr[2];
	}

	FINLINE static Vector3 Lerp(const Vector3& a, const Vector3& b, float t) noexcept
	{
		return Vector3(
			a.x + (b.x - a.x) * t,
			a.y + (b.y - a.y) * t,
			a.z + (b.z - a.z) * t
		);
	}

	FINLINE static Vector3 Cross(const Vector3& a, const Vector3& b) noexcept
	{
		return Vector3(a.arr[1] * b.arr[2] - a.arr[2] * b.arr[1],
					   a.arr[2] * b.arr[0] - a.arr[0] * b.arr[2],
					   a.arr[0] * b.arr[1] - a.arr[1] * b.arr[0]);
	}

	FINLINE static Vector3 Normalize(const Vector3& a) noexcept
	{
		return a / a.Length();
	}

	[[nodiscard]] FINLINE static Vector3 One()		noexcept { return  Vector3(1.0f, 1.0f, 1.0f); }
	[[nodiscard]] FINLINE static Vector3 Zero()		noexcept { return  Vector3(0.0f, 0.0f, 0.0f); }
	[[nodiscard]] FINLINE static Vector3 Up()		noexcept { return  Vector3(0.0f, 1.0f, 0.0f); }
	[[nodiscard]] FINLINE static Vector3 Right()	noexcept { return  Vector3(1.0f, 0.0f, 0.0f); }
	[[nodiscard]] FINLINE static Vector3 Forward()	noexcept { return  Vector3(0.0f, 0.0f, 1.0f); }

	FINLINE Vector3 VECTORCALL operator - () const { return Vector3(-x, -y, -z); }
	FINLINE Vector3 VECTORCALL operator + (const Vector3 b) const { return Vector3(x + b.x, y + b.y, z + b.z); }
	FINLINE Vector3 VECTORCALL operator - (const Vector3 b) const { return Vector3(x - b.x, y - b.y, z - b.z); }
	FINLINE Vector3 VECTORCALL operator * (const Vector3 b) const { return Vector3(x * b.x, y * b.y, z * b.z); }
	FINLINE Vector3 VECTORCALL operator / (const Vector3 b) const { return Vector3(x / b.x, y / b.y, z / b.z); }

	FINLINE Vector3 VECTORCALL operator += (const Vector3 b) { x += b.x; y += b.y; z += b.z; return *this; }
	FINLINE Vector3 VECTORCALL operator -= (const Vector3 b) { x -= b.x; y -= b.y; z -= b.z; return *this; }
	FINLINE Vector3 VECTORCALL operator *= (const Vector3 b) { x *= b.x; y *= b.y; z *= b.z; return *this; }
	FINLINE Vector3 VECTORCALL operator /= (const Vector3 b) { x /= b.x; y /= b.y; z /= b.z; return *this; }

	FINLINE Vector3 operator *  (const float b) const	 { return Vector3(x * b, y * b, z * b);; }
	FINLINE Vector3 operator /  (const float b) const	 { return Vector3(x / b, y / b, z / b); }
	FINLINE Vector3 operator *= (const float b) noexcept { x *= b; y *= b; z *= b;  return *this; }
	FINLINE Vector3 operator /= (const float b) noexcept { x /= b; y /= b; z /= b; return *this; }
};


struct Vector3d
{
	union
	{
		struct { double x, y, z; };
		double arr[3];
		__m256d vec;
	};

	FINLINE Vector3d() : x(0), y(0), z(0) {}
	FINLINE Vector3d(double scale) : x(scale), y(scale), z(scale) {}
	Vector3d(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}
	Vector3d(__m256d _vec) : vec(_vec) {}

	FINLINE double Length() const { return sqrt(LengthSquared()); }
	FINLINE double LengthSquared() const noexcept { return x * x + y * y + z * z; }

	Vector3d& Normalize() { *this /= Length(); return *this; }

	FINLINE static double Length(const Vector3d& vec) { return vec.Length(); }

	FINLINE static double Dot(const Vector3d& a, const Vector3d& b)
	{
		return a.arr[0] * b.arr[0] + a.arr[1] * b.arr[1] + a.arr[2] * b.arr[2];
	}

	FINLINE static float Distance(const Vector3d& a, const Vector3d& b)
	{
		Vector3d diff = a - b;
		diff *= diff;
		return sqrt(diff.x + diff.y + diff.z);
	}

	FINLINE static Vector3d Lerp(const Vector3d& a, const Vector3d& b, float t)
	{
		return Vector3d(
			a.x + (b.x - a.x) * t,
			a.y + (b.y - a.y) * t,
			a.z + (b.z - a.z) * t
		);
	}

	FINLINE static Vector3d Cross(const Vector3d& a, const Vector3d& b)
	{
		return Vector3d(a.arr[1] * b.arr[2] - a.arr[2] * b.arr[1],
					    a.arr[2] * b.arr[0] - a.arr[0] * b.arr[2],
					    a.arr[0] * b.arr[1] - a.arr[1] * b.arr[0]);
	}

	FINLINE static Vector3d Normalize(const Vector3d& a)
	{
		return a / a.Length();
	}

	[[nodiscard]] FINLINE static Vector3d One()		noexcept { return  Vector3d(1.0, 1.0, 1.0); }
	[[nodiscard]] FINLINE static Vector3d Zero()	noexcept { return  Vector3d(0.0, 0.0, 0.0); }
	[[nodiscard]] FINLINE static Vector3d Up()		noexcept { return  Vector3d(0.0, 1.0, 0.0); }
	[[nodiscard]] FINLINE static Vector3d Right()	noexcept { return  Vector3d(1.0, 0.0, 0.0); }
	[[nodiscard]] FINLINE static Vector3d Forward()	noexcept { return  Vector3d(0.0, 0.0, 1.0); }

	FINLINE Vector3d VECTORCALL operator - () const { return _mm256_mul_pd(vec, _mm256_set1_pd(-1.0)); }
	FINLINE Vector3d VECTORCALL operator + (const Vector3d b) const { return _mm256_add_pd(vec, b.vec); }
	FINLINE Vector3d VECTORCALL operator - (const Vector3d b) const { return _mm256_sub_pd(vec, b.vec); }
	FINLINE Vector3d VECTORCALL operator * (const Vector3d b) const { return _mm256_mul_pd(vec, b.vec); }
	FINLINE Vector3d VECTORCALL operator / (const Vector3d b) const { return _mm256_div_pd(vec, b.vec); }

	FINLINE Vector3d VECTORCALL operator += (const Vector3d b) { vec = _mm256_add_pd(vec, b.vec); return *this; }
	FINLINE Vector3d VECTORCALL operator -= (const Vector3d b) { vec = _mm256_sub_pd(vec, b.vec); return *this; }
	FINLINE Vector3d VECTORCALL operator *= (const Vector3d b) { vec = _mm256_mul_pd(vec, b.vec); return *this; }
	FINLINE Vector3d VECTORCALL operator /= (const Vector3d b) { vec = _mm256_div_pd(vec, b.vec); return *this; }

	FINLINE Vector3d operator *	 (const double b) const { return _mm256_mul_pd(vec, _mm256_set1_pd(b)); }
	FINLINE Vector3d operator /	 (const double b) const { return _mm256_div_pd(vec, _mm256_set1_pd(b)); }
	FINLINE Vector3d operator *= (const double b) noexcept { vec = _mm256_mul_pd(vec, _mm256_set1_pd(b)); return *this; }
	FINLINE Vector3d operator /= (const double b) noexcept { vec = _mm256_div_pd(vec, _mm256_set1_pd(b)); return *this; }
};

struct Vector3i
{
	union
	{
		struct { int x, y, z; };
		int arr[3];
		__m128i vec;
	};

	FINLINE Vector3i() : x(0), y(0), z(0) {}
	FINLINE Vector3i(int scale) : x(scale), y(scale), z(scale) {}
	Vector3i(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}
	Vector3i(__m128i _vec) : vec(_vec) {}

	FINLINE float Length() const { return sqrtf(LengthSquared()); }
	FINLINE float LengthSquared() const { return x * x + y * y + z * z; }

	Vector3i& Normalize() { *this /= Length(); return *this; }

	[[nodiscard]] FINLINE static Vector3i One() noexcept { return  Vector3i(1, 1, 1); }
	[[nodiscard]] FINLINE static Vector3i Zero() noexcept { return  Vector3i(0, 0, 0); }

	FINLINE Vector3i VECTORCALL operator - () const { return _mm_mul_epi32(vec, _mm_set1_epi32(-1)); }
	FINLINE Vector3i VECTORCALL operator + (const Vector3i b) const { return _mm_add_epi32(vec, b.vec); }
	FINLINE Vector3i VECTORCALL operator - (const Vector3i b) const { return _mm_sub_epi32(vec, b.vec); }
	FINLINE Vector3i VECTORCALL operator * (const Vector3i b) const { return _mm_mul_epi32(vec, b.vec); }
	FINLINE Vector3i VECTORCALL operator / (const Vector3i b) const { return _mm_div_epi32(vec, b.vec); }

	FINLINE Vector3i VECTORCALL operator += (const Vector3i b) { vec = _mm_add_epi32(vec, b.vec); return *this; }
	FINLINE Vector3i VECTORCALL operator -= (const Vector3i b) { vec = _mm_sub_epi32(vec, b.vec); return *this; }
	FINLINE Vector3i VECTORCALL operator *= (const Vector3i b) { vec = _mm_mul_epi32(vec, b.vec); return *this; }
	FINLINE Vector3i VECTORCALL operator /= (const Vector3i b) { vec = _mm_div_epi32(vec, b.vec); return *this; }

	FINLINE Vector3i operator *	 (const int b) const { return _mm_mul_epi32(vec, _mm_set1_epi32(b)); }
	FINLINE Vector3i operator /	 (const int b) const { return _mm_div_epi32(vec, _mm_set1_epi32(b)); }
	FINLINE Vector3i operator *= (const int b) noexcept { vec = _mm_mul_epi32(vec, _mm_set1_epi32(b)); return *this; }
	FINLINE Vector3i operator /= (const int b) noexcept { vec = _mm_div_epi32(vec, _mm_set1_epi32(b)); return *this; }
};

// --- Convert ---

[[nodiscard]] FINLINE Vector3i VECTORCALL ToVec3i(const Vector3 vec3f)  noexcept { return _mm_cvtps_epi32(vec3f.vec()); }
[[nodiscard]] FINLINE Vector3i VECTORCALL ToVec3i(const Vector3d vec3d) noexcept { return _mm256_cvtpd_epi32(vec3d.vec); }

[[nodiscard]] FINLINE Vector3 VECTORCALL ToVec3f(const Vector3d vec3d) noexcept { return _mm256_cvtpd_ps(vec3d.vec); }
[[nodiscard]] FINLINE Vector3 VECTORCALL ToVec3f(const Vector3i vec3i) noexcept { return _mm_cvtepi32_ps(vec3i.vec); }

[[nodiscard]] FINLINE Vector3d VECTORCALL ToVec3d(const Vector3i vec3i) noexcept { return _mm256_cvtepi32_pd(vec3i.vec); }
[[nodiscard]] FINLINE Vector3d VECTORCALL ToVec3d(const Vector3 vec3d)  noexcept { return _mm256_cvtps_pd	(vec3d.vec()); }

// --- Angle ---

[[nodiscard]] FINLINE Vector3 VECTORCALL RadianToDegree(const Vector3 radian) {
	return radian * RadToDeg;
}

[[nodiscard]] FINLINE Vector3 VECTORCALL DegreeToRadian(const Vector3 degree) {
	return degree * DegToRad;
}

FINLINE void VECTORCALL RadianToDegree(const Vector3 radian, Vector3& degree) {
	degree = radian * RadToDeg;
}

FINLINE void VECTORCALL DegreeToRadian(const Vector3 degree, Vector3& radian) {
	radian = degree * DegToRad;
}

AMATH_END_NAMESPACE