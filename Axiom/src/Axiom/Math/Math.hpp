#pragma once

#include <cmath>
#include "../Core/Common.hpp"
#include "All.hpp"

namespace Math
{
	template<typename T>
	static constexpr FINLINE T Lerp(T min, T max, T alpha)
	{
		return min + (max - min) * alpha;
	}

	template<typename T, typename Other>
	constexpr FINLINE T Min(T left, Other right)
	{
		return left <= right ? left : right;
	}

	/*template<>
	constexpr FINLINE Vector3 Min<Vector3, Vector3>(Vector3 left, Vector3 right)
	{
		return { Min(left.x, right.x), Min(left.y, right.y), Min(left.z, right.z) };
	}*/

	template<typename T, typename Other>
	constexpr FINLINE T Max(T left, Other right)
	{
		return left >= right ? left : right;
	}

	/*template<>
	constexpr FINLINE Vector3 Max<Vector3, Vector3>(Vector3 left, Vector3 right)
	{
		return { Max(left.x, right.x), Max(left.y, right.y), Max(left.z, right.z) };
	}*/

	template<typename T, typename TMin, typename TMax>
	static constexpr FINLINE T Clamp(T value, TMin min, TMax max)
	{
		return Max(min, Min(max, value));
	}

	template<typename T, typename Min, typename Max>
	static constexpr FINLINE T WrapAround(T value, Min min, Max max)
	{
		if (value > max)
			return (T)min;

		if (value < min)
			return (T)max;

		return value;
	}

	template<typename T>
	static constexpr FINLINE T Saturate(T value)
	{
		return Clamp(value, T(0), T(1));
	};

	template<typename T>
	static constexpr FINLINE T Abs(T value)
	{
		return (value >= T(0)) ? value : -value;
	};

	template<typename T>
	FINLINE int RoundInt(T value)
	{
		return (int) round((T)value);
	}

	template<typename T>
	FINLINE int CeilToInt(T value)
	{
		return (int) ceil((T)value);
	}

	static FINLINE void RandInit(int32 seed)
	{
		srand(seed);
	}

	FINLINE float  IsZero(const float x)	noexcept { return fabsf(x) > 1e-10f; }
	FINLINE double IsZero(const double x)	noexcept { return fabs(x)  > 1e-10; }

	template<typename RealT>
	FINLINE RealT Repeat(const RealT t, const RealT length) noexcept
	{
		return Clamp(t - floorf(t / length) * length, 0.0, length);
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
		return f + 1.0f * 0.5;
	}

	// (-1,1) to (0,1) range
	FINLINE double SinRangeToZeroOne(double d) {
		return d + 1.0 * 0.5;
	}

	FINLINE float Remap(float x, float inMin, float inMax, float outMin, float outMax)
	{
		return outMin + (outMax - outMin) * ((x - inMin) / (inMax - inMin));
	}

	FINLINE float RSqrt(float number)
	{
		long i;
		float x2, y;

		x2 = number * 0.5F;
		y = number;
		i = *(long*)&y;                       // evil floating point bit level hacking
		i = 0x5f3759df - (i >> 1);               // what the fuck?
		y = *(float*)&i;
		y = y * (1.5f - (x2 * y * y));   // 1st iteration
		return y;
	}

	// Code below adapted from DirectX::Math
	FINLINE void ScalarSinCos(float* pSin, float* pCos, float Value) noexcept
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

	static FINLINE int32 Rand()
	{
		return rand();
	}

	static FINLINE float FRand()
	{
		constexpr int32 randMax = 0x00ffffff < RAND_MAX ? 0x00ffffff : RAND_MAX;
		return (Rand() & randMax) / (float)randMax;
	}
}
