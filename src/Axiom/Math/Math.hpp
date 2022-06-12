#pragma once
#include <math.h>
#include <immintrin.h>

#ifndef FINLINE 
#	ifndef _MSC_VER 
#		define FINLINE  inline
#	else
#		define FINLINE __forceinline
#	endif
#endif

#ifndef VECTORCALL
#	ifndef _MSC_VER
#		define VECTORCALL 
#	else
#	   define VECTORCALL __vectorcall
#	endif
#endif

#define AMATH_NAMESPACE namespace ax {
#define AMATH_END_NAMESPACE }

AMATH_NAMESPACE

// constants
constexpr float PI = 3.141592653;
constexpr float RadToDeg = 180.0f / PI;
constexpr float DegToRad = PI / 180.0f;

template<typename T> FINLINE T Max(const T a, const T b)	{ return a > b ? a : b; }
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


FINLINE double Lerp(double from, double to, double t)
{
	return from + (to - from) * t;
}

FINLINE double LerpAngle(const double a, const double b, const double t) noexcept
{
	double delta = Repeat((b - a), 360);
	if (delta > 180) delta -= 360;
	return a + delta * Clamp(t, 0.0, 1.0);
}

AMATH_END_NAMESPACE