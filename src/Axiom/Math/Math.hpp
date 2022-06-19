#pragma once

#include <cmath>
#include <immintrin.h>
#include "Axiom/Core/Common.hpp"

#ifndef AMATH_NAMESPACE
	#define AMATH_NAMESPACE namespace ax {
	#define AMATH_END_NAMESPACE }
#endif

#ifndef AXGLOBALCONST
#	if _MSC_VER
#		define AXGLOBALCONST extern const __declspec(selectany)
#	elif defined(__GNUC__) && !defined(__MINGW32__)
#		define AXGLOBALCONST extern const __attribute__((weak))
#	endif
#endif

AMATH_NAMESPACE
// constants
constexpr float PI = 3.141592653;
constexpr float RadToDeg = 180.0f / PI;
constexpr float DegToRad = PI / 180.0f;
constexpr float OneDivPI = 1.0f / PI;
constexpr float PIDiv2   = PI   / 2.0f;
constexpr float TwoPI    = PI   * 2.0f;

template<typename T> FINLINE T Max(const T a, const T b) { return a > b ? a : b; }
template<typename T> FINLINE T Min(const T a, const T b) { return a < b ? a : b; }
template<typename T> FINLINE T Clamp(const T x, const T a, const T b) { return Max(a, Min(b, x)); }

FINLINE float  IsZero(const float x)	noexcept { return fabsf(x) > 1e-10f; }
FINLINE double IsZero(const double x)	noexcept { return fabs(x)  > 1e-10; }

FINLINE float Repeat(const float t, const float length) noexcept
{
	return Clamp(t - floorf(t / length) * length, 0.0f, length);
}

FINLINE float Lerp(const float from, const float to, const float t) noexcept
{
	return from + (to - from) * t;
}

FINLINE float LerpAngle(const float a, const float b, const float t) noexcept
{
	float delta = Repeat((b - a), 360);
	if (delta > 180) delta -= 360;
	return a + delta * Clamp(t, 0.0f, 1.0f);
}

FINLINE double Lerp(const double from, const double to, const double t)
{
	return from + (to - from) * t;
}

FINLINE double LerpAngle(const double a, const double b, const double t) noexcept
{
	double delta = Repeat((b - a), 360);
	if (delta > 180) delta -= 360;
	return a + delta * Clamp(t, 0.0, 1.0);
}

// Code below adapted from DirectX::Math
inline void ScalarSinCos(float* pSin, float* pCos, float  Value) noexcept
{
	float quotient = (1.0f / PI) * Value;

	if (Value >= 0.0f)
		quotient = float(int(quotient + 0.5f));
	else quotient = float(int(quotient - 0.5f));

	float y = Value - TwoPI * quotient;
	// Map y to [-pi/2,pi/2] with sin(y) = sin(Value).
	float sign;
	if (y > PIDiv2) {
		y = PI - y;
		sign = -1.0f;
	}
	else if (y < -PIDiv2) {
		y = -PI - y;
		sign = -1.0f;
	}
	else {
		sign = +1.0f;
	}
	float y2 = y * y;
	// 11-degree minimax approximation
	*pSin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;
	// 10-degree minimax approximation
	float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
	*pCos = sign * p;
}

struct Vector4UI
{
	union
	{
		struct { uint32 x, y, z, w; };
		__m128 vec;
	};

	inline operator __m128 () const noexcept { return vec; }
	inline operator __m128i() const noexcept { return _mm_castps_si128(vec); }
	inline operator __m128d() const noexcept { return _mm_castps_pd(vec); }

	FINLINE Vector4UI() : x(0), y(0), z(0) {}
	FINLINE Vector4UI(uint32 _x, uint32 _y, uint32 _z, uint32 _w) : x(_x), y(_y), z(_z), w(_w) {}
};

struct Vector432F
{
	union
	{
		struct { float x, y, z, w; };
		__m128 vec;
	};

	inline operator __m128 () const noexcept { return vec; }
	inline operator __m128i() const noexcept { return _mm_castps_si128(vec); }
	inline operator __m128d() const noexcept { return _mm_castps_pd(vec); }

	FINLINE Vector432F() : x(0), y(0), z(0) {}
	FINLINE Vector432F(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
};

constexpr uint32_t AX_SELECT_0 = 0x00000000;
constexpr uint32_t AX_SELECT_1 = 0xFFFFFFFF;

AXGLOBALCONST Vector4UI g_XMSelect1000 = { AX_SELECT_1, AX_SELECT_0, AX_SELECT_0, AX_SELECT_0 };
AXGLOBALCONST Vector4UI g_XMSelect1100 = { AX_SELECT_1, AX_SELECT_1, AX_SELECT_0, AX_SELECT_0 };
AXGLOBALCONST Vector4UI g_XMSelect1110 = { AX_SELECT_1, AX_SELECT_1, AX_SELECT_1, AX_SELECT_0 };
AXGLOBALCONST Vector4UI g_XMSelect1011 = { AX_SELECT_1, AX_SELECT_0, AX_SELECT_1, AX_SELECT_1 };

AXGLOBALCONST Vector432F g_XMIdentityR0 = { 1.0f, 0.0f, 0.0f, 0.0f };
AXGLOBALCONST Vector432F g_XMIdentityR1 = { 0.0f, 1.0f, 0.0f, 0.0f };
AXGLOBALCONST Vector432F g_XMIdentityR2 = { 0.0f, 0.0f, 1.0f, 0.0f };
AXGLOBALCONST Vector432F g_XMIdentityR3 = { 0.0f, 0.0f, 0.0f, 1.0f };

AXGLOBALCONST Vector4UI g_XMMaskXY = { 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000 };
AXGLOBALCONST Vector4UI g_XMMask3 = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000 };
AXGLOBALCONST Vector4UI g_XMMaskX = { 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000 };
AXGLOBALCONST Vector4UI g_XMMaskY = { 0x00000000, 0xFFFFFFFF, 0x00000000, 0x00000000 };
AXGLOBALCONST Vector4UI g_XMMaskZ = { 0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000 };
AXGLOBALCONST Vector4UI g_XMMaskW = { 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF };

AXGLOBALCONST Vector432F g_XMOne = { 1.0f, 1.0f, 1.0f, 1.0f };
AXGLOBALCONST Vector432F g_XMOne3 = { 1.0f, 1.0f, 1.0f, 0.0f };
AXGLOBALCONST Vector432F g_XMZero = { 0.0f, 0.0f, 0.0f, 0.0f };
AXGLOBALCONST Vector432F g_XMTwo = { 2.0f, 2.0f, 2.0f, 2.0f };
AXGLOBALCONST Vector432F g_XMFour = { 4.0f, 4.0f, 4.0f, 4.0f };
AXGLOBALCONST Vector432F g_XMSix = { 6.0f, 6.0f, 6.0f, 6.0f };
AXGLOBALCONST Vector432F g_XMNegativeOne = { -1.0f, -1.0f, -1.0f, -1.0f };
AXGLOBALCONST Vector432F g_XMOneHalf = { 0.5f, 0.5f, 0.5f, 0.5f };

FINLINE __m128 VECTORCALL SSESelect(const __m128 V1, const __m128 V2, const __m128& Control)
{
	__m128 vTemp1 = _mm_andnot_ps(Control, V1);
	__m128 vTemp2 = _mm_and_ps(V2, Control);
	return _mm_or_ps(vTemp1, vTemp2);
}

FINLINE __m128 VECTORCALL SSESplatX(const __m128 V1)  { return _mm_permute_ps(V1, _MM_SHUFFLE(0, 0, 0, 0)); }
FINLINE __m128 VECTORCALL SSESplatY(const __m128 V1)  { return _mm_permute_ps(V1, _MM_SHUFFLE(1, 1, 1, 1)); }
FINLINE __m128 VECTORCALL SSESplatZ(const __m128 V1)  { return _mm_permute_ps(V1, _MM_SHUFFLE(2, 2, 2, 2)); }
FINLINE __m128 VECTORCALL SSESplatW(const __m128 V1)  { return _mm_permute_ps(V1, _MM_SHUFFLE(3, 3, 3, 3)); }

FINLINE __m128 VECTORCALL SSEVector3Normalize(const __m128 V)
{
	__m128 vTemp = _mm_dp_ps(V, V, 0x7f);
	__m128 vResult = _mm_rsqrt_ps(vTemp);
	return _mm_mul_ps(vResult, V);
}

FINLINE __m128 VECTORCALL SSEVector3Cross(const __m128 V1, const __m128  V2)
{
	__m128 vTemp1 = _mm_permute_ps(V1, _MM_SHUFFLE(3, 0, 2, 1));
	__m128 vTemp2 = _mm_permute_ps(V2, _MM_SHUFFLE(3, 1, 0, 2));
	__m128 vResult = _mm_mul_ps(vTemp1, vTemp2);
	vTemp1 	= _mm_permute_ps(vTemp1, _MM_SHUFFLE(3, 0, 2, 1));
	vTemp2 	= _mm_permute_ps(vTemp2, _MM_SHUFFLE(3, 1, 0, 2));
	vResult = _mm_fnmadd_ps(vTemp1, vTemp2, vResult);
	// Set w to zero
	return _mm_and_ps(vResult, g_XMMask3);
}

FINLINE __m128 VECTORCALL SSEVector3Dot(const __m128 V1, const __m128 V2)
{
	__m128 vDot = _mm_mul_ps(V1, V2);
	__m128 vTemp = _mm_permute_ps(vDot, _MM_SHUFFLE(2, 1, 2, 1));
	vDot = _mm_add_ss(vDot, vTemp);
	vTemp = _mm_permute_ps(vTemp, _MM_SHUFFLE(1, 1, 1, 1));
	vDot = _mm_add_ss(vDot, vTemp);
	// Splat x
	return _mm_permute_ps(vDot, _MM_SHUFFLE(0, 0, 0, 0));
}

AMATH_END_NAMESPACE













