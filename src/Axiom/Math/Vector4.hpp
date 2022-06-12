#pragma once
#include "Math.hpp"
#include <emmintrin.h>

AMATH_NAMESPACE

struct Vector4
{
	union
	{
		struct { float x, y, z, w; };
		float arr[4];
		__m128 vec;
	};

	FINLINE Vector4() : x(0), y(0), z(0), w(0) {}
	FINLINE Vector4(float scale) : x(scale), y(scale), z(scale), w(scale) {}
	Vector4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
	Vector4(__m128 _vec) : vec(_vec) {}

	FINLINE static __m128 VECTORCALL Dot(const Vector4 V1, const Vector4 V2)
	{
		__m128 vTemp2 = V2.vec;
		__m128 vTemp = _mm_mul_ps(V1.vec, vTemp2);
		vTemp2 = _mm_shuffle_ps(vTemp2, vTemp, _MM_SHUFFLE(1, 0, 0, 0)); // Copy X to the Z position and Y to the W position
		vTemp2 = _mm_add_ps(vTemp2, vTemp);          // Add Z = X+Z; W = Y+W;
		vTemp = _mm_shuffle_ps(vTemp, vTemp2, _MM_SHUFFLE(0, 3, 0, 0));  // Copy W to the Z position
		vTemp = _mm_add_ps(vTemp, vTemp2);           // Add Z and W together
		return _mm_shuffle_ps(vTemp, vTemp, _MM_SHUFFLE(2, 2, 2, 2));    // Splat Z and return
	}

	FINLINE static __m128 VECTORCALL Normalize(const __m128 V)
	{
		__m128 vLengthSq = _mm_mul_ps(V, V);
		__m128 vTemp = _mm_shuffle_ps(vLengthSq, vLengthSq, _MM_SHUFFLE(3, 2, 3, 2));
		vLengthSq = _mm_add_ps(vLengthSq, vTemp);
		vLengthSq = _mm_shuffle_ps(vLengthSq, vLengthSq, _MM_SHUFFLE(1, 0, 0, 0));
		vTemp = _mm_shuffle_ps(vTemp, vLengthSq, _MM_SHUFFLE(3, 3, 0, 0));
		vLengthSq = _mm_add_ps(vLengthSq, vTemp);
		vLengthSq = _mm_shuffle_ps(vLengthSq, vLengthSq, _MM_SHUFFLE(2, 2, 2, 2));
		__m128 vResult = _mm_sqrt_ps(vLengthSq);
		__m128 vZeroMask = _mm_setzero_ps();
		vZeroMask = _mm_cmpneq_ps(vZeroMask, vResult);
		vLengthSq = _mm_cmpneq_ps(vLengthSq, _mm_set_ps(0x7F800000, 0x7F800000, 0x7F800000, 0x7F800000));
		vResult = _mm_div_ps(V, vResult);
		vResult = _mm_and_ps(vResult, vZeroMask);
		__m128 vTemp1 = _mm_andnot_ps(vLengthSq, _mm_set_ps(0x7FC00000, 0x7FC00000, 0x7FC00000, 0x7FC00000));
		__m128 vTemp2 = _mm_and_ps(vResult, vLengthSq);
		vResult = _mm_or_ps(vTemp1, vTemp2);
		return vResult;
	}

	FINLINE Vector4 VECTORCALL operator + (const Vector4 b) const { return _mm_add_ps(vec, b.vec); }
	FINLINE Vector4 VECTORCALL operator - (const Vector4 b) const { return _mm_sub_ps(vec, b.vec); }
	FINLINE Vector4 VECTORCALL operator * (const Vector4 b) const { return _mm_mul_ps(vec, b.vec); }
	FINLINE Vector4 VECTORCALL operator / (const Vector4 b) const { return _mm_div_ps(vec, b.vec); }

	FINLINE Vector4 VECTORCALL operator += (const Vector4 b) { vec = _mm_add_ps(vec, b.vec); return *this; }
	FINLINE Vector4 VECTORCALL operator -= (const Vector4 b) { vec = _mm_sub_ps(vec, b.vec); return *this; }
	FINLINE Vector4 VECTORCALL operator *= (const Vector4 b) { vec = _mm_mul_ps(vec, b.vec); return *this; }
	FINLINE Vector4 VECTORCALL operator /= (const Vector4 b) { vec = _mm_div_ps(vec, b.vec); return *this; }

	FINLINE Vector4 operator *  (const float b) const { return _mm_mul_ps(vec, _mm_set_ps1(b)); }
	FINLINE Vector4 operator /  (const float b) const { return _mm_div_ps(vec, _mm_set_ps1(b)); }
	FINLINE Vector4 operator *= (const float b) { vec = _mm_mul_ps(vec, _mm_set_ps1(b)); return *this; }
	FINLINE Vector4 operator /= (const float b) { vec = _mm_div_ps(vec, _mm_set_ps1(b)); return *this; }
};

struct Vector4d
{
	union
	{
		struct { double x, y, z, w; };
		double arr[4];
		__m256d vec;
	};

	FINLINE Vector4d() : x(0), y(0), z(0), w(0) {}
	FINLINE Vector4d(double scale) : x(scale), y(scale), z(scale), w(scale) {}
	Vector4d(double _x, double _y, double _z, double _w) : x(_x), y(_y), z(_z), w(_w) {}
	Vector4d(__m256d _vec) : vec(_vec) {}

	FINLINE static __m256d VECTORCALL Dot(const Vector4d V1, const Vector4d& V2)
	{
		__m256d vTemp2 = V2.vec;
		__m256d vTemp = _mm256_mul_pd(V1.vec, vTemp2);
		vTemp2	= _mm256_shuffle_pd(vTemp2, vTemp, _MM_SHUFFLE(1, 0, 0, 0)); // Copy X to the Z position and Y to the W position
		vTemp2	= _mm256_add_pd(vTemp2, vTemp);          // Add Z = X+Z; W = Y+W;
		vTemp	= _mm256_shuffle_pd(vTemp, vTemp2, _MM_SHUFFLE(0, 3, 0, 0));  // Copy W to the Z position
		vTemp	= _mm256_add_pd(vTemp, vTemp2);           // Add Z and W together
		return _mm256_shuffle_pd(vTemp, vTemp, _MM_SHUFFLE(2, 2, 2, 2));    // Splat Z and return
	}

	FINLINE static __m256d VECTORCALL Normalize(const __m256d  V)
	{
		__m256d vLengthSq	= _mm256_mul_pd(V, V);
		__m256d vTemp		= _mm256_shuffle_pd(vLengthSq, vLengthSq, _MM_SHUFFLE(3, 2, 3, 2));
		vLengthSq			= _mm256_add_pd(vLengthSq, vTemp);
		vLengthSq			= _mm256_shuffle_pd(vLengthSq, vLengthSq, _MM_SHUFFLE(1, 0, 0, 0));
		vTemp				= _mm256_shuffle_pd(vTemp, vLengthSq, _MM_SHUFFLE(3, 3, 0, 0));
		vLengthSq			= _mm256_add_pd(vLengthSq, vTemp);
		vLengthSq			= _mm256_shuffle_pd(vLengthSq, vLengthSq, _MM_SHUFFLE(2, 2, 2, 2));
		__m256d vResult		= _mm256_sqrt_pd(vLengthSq);
		__m256d vZeroMask	= _mm256_setzero_pd();
		vZeroMask			= _mm256_set1_pd(1.0);// _mm256_cmpneq_pd(vZeroMask, vResult);
		vLengthSq			= _mm256_set1_pd(1.0);// _mm256_cmpeq(vLengthSq, _mm256_set_pd(0x7F800000, 0x7F800000, 0x7F800000, 0x7F800000));
		vResult				= _mm256_div_pd(V, vResult);
		vResult				= _mm256_and_pd(vResult, vZeroMask);
		__m256d vTemp1		= _mm256_andnot_pd(vLengthSq, _mm256_set_pd(0x7FC00000, 0x7FC00000, 0x7FC00000, 0x7FC00000));
		__m256d vTemp2		= _mm256_and_pd(vResult, vLengthSq);
		vResult				= _mm256_or_pd(vTemp1, vTemp2);
		return vResult;
	}

	FINLINE Vector4d VECTORCALL operator + (const Vector4d b) const { return _mm256_add_pd(vec, b.vec); }
	FINLINE Vector4d VECTORCALL operator - (const Vector4d b) const { return _mm256_sub_pd(vec, b.vec); }
	FINLINE Vector4d VECTORCALL operator*  (const Vector4d b) const { return _mm256_mul_pd(vec, b.vec); }
	FINLINE Vector4d VECTORCALL operator / (const Vector4d b) const { return _mm256_div_pd(vec, b.vec); }

	FINLINE Vector4d VECTORCALL operator += (const Vector4d b) { vec = _mm256_add_pd(vec, b.vec); return *this; }
	FINLINE Vector4d VECTORCALL operator -= (const Vector4d b) { vec = _mm256_sub_pd(vec, b.vec); return *this; }
	FINLINE Vector4d VECTORCALL operator *= (const Vector4d b) { vec = _mm256_mul_pd(vec, b.vec); return *this; }
	FINLINE Vector4d VECTORCALL operator /= (const Vector4d b) { vec = _mm256_div_pd(vec, b.vec); return *this; }

	FINLINE Vector4d operator *  (const float b) const { return _mm256_mul_pd(vec, _mm256_set1_pd(b)); }
	FINLINE Vector4d operator /  (const float b) const { return _mm256_div_pd(vec, _mm256_set1_pd(b)); }
	FINLINE Vector4d operator *= (const float b) noexcept { vec = _mm256_mul_pd(vec, _mm256_set1_pd(b)); return *this; }
	FINLINE Vector4d operator /= (const float b) noexcept { vec = _mm256_div_pd(vec, _mm256_set1_pd(b)); return *this; }
};

struct Vector4i
{
	union
	{
		struct { int x, y, z, w; };
		int arr[4];
		__m128i vec;
	};

	FINLINE Vector4i() : x(0), y(0), z(0), w(0) {}
	FINLINE Vector4i(int scale) : x(scale), y(scale), z(scale), w(scale) {}
	Vector4i(int _x, int _y, int _z, int _w) : x(_x), y(_y), z(_z), w(_w) {}
	Vector4i(__m128i _vec) : vec(_vec) {}

	FINLINE Vector4i VECTORCALL operator + (const Vector4i b) const { return _mm_add_epi32(vec, b.vec); }
	FINLINE Vector4i VECTORCALL operator - (const Vector4i b) const { return _mm_sub_epi32(vec, b.vec); }
	FINLINE Vector4i VECTORCALL operator* (const Vector4i b) const { return _mm_mul_epi32(vec, b.vec); }
	FINLINE Vector4i VECTORCALL operator / (const Vector4i b) const { return _mm_div_epi32(vec, b.vec); }

	FINLINE Vector4i VECTORCALL operator += (const Vector4i b) { vec = _mm_add_epi32(vec, b.vec); return *this; }
	FINLINE Vector4i VECTORCALL operator -= (const Vector4i b) { vec = _mm_sub_epi32(vec, b.vec); return *this; }
	FINLINE Vector4i VECTORCALL operator *= (const Vector4i b) { vec = _mm_mul_epi32(vec, b.vec); return *this; }
	FINLINE Vector4i VECTORCALL operator /= (const Vector4i b) { vec = _mm_div_epi32(vec, b.vec); return *this; }

	FINLINE Vector4i operator *  (const int b) const { return _mm_mul_epi32(vec, _mm_set1_epi32(b)); }
	FINLINE Vector4i operator /  (const int b) const { return _mm_div_epi32(vec, _mm_set1_epi32(b)); }
	FINLINE Vector4i operator *= (const int b) noexcept { vec = _mm_mul_epi32(vec, _mm_set1_epi32(b)); return *this; }
	FINLINE Vector4i operator /= (const int b) noexcept { vec = _mm_div_epi32(vec, _mm_set1_epi32(b)); return *this; }
};

FINLINE Vector4i VECTORCALL ToVec4i(const Vector4 vec3f)	{ return _mm_cvtps_epi32(vec3f.vec); }
FINLINE Vector4i VECTORCALL ToVec4i(const Vector4d vec3d) { return _mm256_cvtpd_epi32(vec3d.vec); }

FINLINE Vector4 VECTORCALL ToVec4f(const Vector4d vec3d) { return _mm256_cvtpd_ps(vec3d.vec); }
FINLINE Vector4 VECTORCALL ToVec4f(const Vector4i vec3i) { return _mm_cvtepi32_ps(vec3i.vec); }

FINLINE Vector4d VECTORCALL ToVec4d(const Vector4i vec3i) { return _mm256_cvtepi32_pd(vec3i.vec); }
FINLINE Vector4d VECTORCALL ToVec4d(const Vector4 vec3d)	{ return _mm256_cvtps_pd(vec3d.vec);	}

AMATH_END_NAMESPACE