#pragma once

#include <math.h>
#include "../Core/Common.hpp"

#ifndef AMATH_NAMESPACE
	#define AMATH_NAMESPACE namespace ax {
	#define AMATH_END_NAMESPACE }
#endif

#ifndef AXGLOBALCONST
#	if _MSC_VER
#		define AXGLOBALCONST extern const __declspec(selectany)
#	elif defined(__GNUC__) && !defined(__MINGW32__)
#		define AXGLOBALCONST extern const __attribute__((weak))
#   else 
#       define AXGLOBALCONST 
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

template<typename T> FINLINE T RemapMinusOneOne();

FINLINE float  IsZero(const float x)	noexcept { return fabsf(x) > 1e-10f; }
FINLINE double IsZero(const double x)	noexcept { return fabs(x)  > 1e-10; }

template<typename RealT>
FINLINE RealT Repeat(const RealT t, const RealT length) noexcept
{
	return Clamp(t - floorf(t / length) * length, 0.0, length);
}

template<typename RealT>
FINLINE RealT Lerp(const RealT from, const RealT to, const RealT t) noexcept
{
	return from + (to - from) * t;
}

template<typename RealT>
FINLINE float LerpAngle(const RealT a, const RealT b, const RealT t) noexcept
{
	float delta = Repeat((b - a), 360);
	if (delta > 180) delta -= 360;
	return a + delta * Clamp(t, 0.0, 1.0);
}

// (0,1) to (-1,1) range
FINLINE float ZeroOneToSinRange(float f) {
	return f * 2.0f - 1.0f;
}

// (0,1) to (-1,1) range
FINLINE double ZeroOneToSinRange(double d) {
	return d * 2.0 - 1.0;
}

// (-1,1) to (0,1) range
FINLINE float SinRangeToZeroOne(float f) {
	return f + 1.0f / 2.0f;
}

// (-1,1) to (0,1) range
FINLINE double SinRangeToZeroOne(double d) {
	return f + 1.0 / 2.0;
}

FINLINE float Remap(float x, float inMin, float inMax, float outMin, float outMax)
{
	return outMin + (outMax - outMin) * ((x - inMin) / (inMax - inMin));
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

AMATH_END_NAMESPACE













