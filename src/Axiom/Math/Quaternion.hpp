#pragma once
#include "Math.hpp"
#include "Vector3.hpp"

AMATH_NAMESPACE

struct Quaternion
{
	union
	{
		struct { float x, y, z, w; };
		float arr[4];
		__m128 vec;
	};

	Quaternion() : x(0), y(0), z(0), w(0) {}
	Quaternion(float scale) : x(scale), y(scale), z(scale), w(scale) {}
	Quaternion(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
	VECTORCALL Quaternion(__m128 _vec) : vec(_vec) {}

	inline static __m128 VECTORCALL Mul(const __m128 Q1, const __m128 Q2) noexcept
	{
		static const __m128 ControlWZYX = { 1.0f,-1.0f, 1.0f,-1.0f };
		static const __m128 ControlZWXY = { 1.0f, 1.0f,-1.0f,-1.0f };
		static const __m128 ControlYXWZ = { -1.0f, 1.0f, 1.0f,-1.0f };
		__m128 Q2X = Q2, Q2Y = Q2, Q2Z = Q2, vResult = Q2;
		vResult = _mm_shuffle_ps(vResult, vResult, _MM_SHUFFLE(3, 3, 3, 3));
		Q2X = _mm_shuffle_ps(Q2X, Q2X, _MM_SHUFFLE(0, 0, 0, 0));
		Q2Y = _mm_shuffle_ps(Q2Y, Q2Y, _MM_SHUFFLE(1, 1, 1, 1));
		Q2Z = _mm_shuffle_ps(Q2Z, Q2Z, _MM_SHUFFLE(2, 2, 2, 2));
		vResult = _mm_mul_ps(vResult, Q1);
		__m128 Q1Shuffle = Q1;
		Q1Shuffle = _mm_shuffle_ps(Q1Shuffle, Q1Shuffle, _MM_SHUFFLE(0, 1, 2, 3));
		Q2X = _mm_mul_ps(Q2X, Q1Shuffle);
		Q1Shuffle = _mm_shuffle_ps(Q1Shuffle, Q1Shuffle, _MM_SHUFFLE(2, 3, 0, 1));
		Q2X = _mm_mul_ps(Q2X, ControlWZYX);
		Q2Y = _mm_mul_ps(Q2Y, Q1Shuffle);
		Q1Shuffle = _mm_shuffle_ps(Q1Shuffle, Q1Shuffle, _MM_SHUFFLE(0, 1, 2, 3));
		Q2Y = _mm_mul_ps(Q2Y, ControlZWXY);
		Q2Z = _mm_mul_ps(Q2Z, Q1Shuffle);
		vResult = _mm_add_ps(vResult, Q2X);
		Q2Z = _mm_mul_ps(Q2Z, ControlYXWZ);
		Q2Y = _mm_add_ps(Q2Y, Q2Z);
		vResult = _mm_add_ps(vResult, Q2Y);
		return vResult;
	}

	FINLINE static __m128 VECTORCALL Dot(const Quaternion V1, const Quaternion V2) noexcept
	{
		__m128 vTemp2 = V2.vec;
		__m128 vTemp = _mm_mul_ps(V1.vec, vTemp2);
		vTemp2 = _mm_shuffle_ps(vTemp2, vTemp, _MM_SHUFFLE(1, 0, 0, 0)); // Copy X to the Z position and Y to the W position
		vTemp2 = _mm_add_ps(vTemp2, vTemp);          // Add Z = X+Z; W = Y+W;
		vTemp = _mm_shuffle_ps(vTemp, vTemp2, _MM_SHUFFLE(0, 3, 0, 0));  // Copy W to the Z position
		vTemp = _mm_add_ps(vTemp, vTemp2);           // Add Z and W together
		return _mm_shuffle_ps(vTemp, vTemp, _MM_SHUFFLE(2, 2, 2, 2));    // Splat Z and return
	}

	inline Quaternion VECTORCALL Slerp(const Quaternion& Q0, const Quaternion& Q1, float t) noexcept
	{
		// Result = Q0 * sin((1.0 - t) * Omega) / sin(Omega) + Q1 * sin(t * Omega) / sin(Omega)
		const __m128 T = _mm_set_ps1(t);
		__m128 Omega, CosOmega, SinOmega, V01, S0, S1, Sign, Result;
		static const __m128 Zero = _mm_set_ps1(0);
		static const __m128 One = _mm_set_ps1(1);
		static const __m128 NegativeOne = _mm_set_ps1(-1);

		static const __m128 OneMinusEpsilon = { 1.0f - 0.00001f, 1.0f - 0.00001f, 1.0f - 0.00001f, 1.0f - 0.00001f };
		static const __m128 SignMask2 = { 0x80000000,0x00000000,0x00000000,0x00000000 };
		static const __m128 MaskXY = { 0xFFFFFFFF,0xFFFFFFFF,0x00000000,0x00000000 };

		CosOmega = Quaternion::Dot(Q0, Q1);

		__m128  Control = _mm_cmplt_ps(CosOmega, Zero);

		__m128  selectTemp1 = _mm_andnot_ps(Control, One);
		__m128  selectTemp2 = _mm_and_ps(NegativeOne, Control);

		Sign = _mm_or_ps(selectTemp1, selectTemp2);
		CosOmega = _mm_mul_ps(CosOmega, Sign);
		Control = _mm_cmplt_ps(CosOmega, OneMinusEpsilon);

		SinOmega = _mm_mul_ps(CosOmega, CosOmega);
		SinOmega = _mm_sub_ps(One, SinOmega);
		SinOmega = _mm_sqrt_ps(SinOmega);

		Omega = _mm_atan2_ps(SinOmega, CosOmega);

		V01 = _mm_shuffle_ps(T, T, _MM_SHUFFLE(2, 3, 0, 1));
		V01 = _mm_and_ps(V01, MaskXY);
		V01 = _mm_xor_ps(V01, SignMask2);
		V01 = _mm_add_ps(_mm_set_ps(1, 0, 0, 0), V01);

		S0 = _mm_mul_ps(V01, Omega);
		S0 = _mm_sin_ps(S0);
		S0 = _mm_div_ps(S0, SinOmega);

		selectTemp1 = _mm_andnot_ps(Control, V01);
		selectTemp2 = _mm_and_ps(S0, Control);

		S0 = _mm_or_ps(selectTemp1, selectTemp2);

		S1 = _mm_shuffle_ps(S0, S0, _MM_SHUFFLE(1, 1, 1, 1));
		S0 = _mm_shuffle_ps(S0, S0, _MM_SHUFFLE(0, 0, 0, 0));

		S1 = _mm_mul_ps(S1, Sign);
		Result = _mm_mul_ps(Q0.vec, S0);
		S1 = _mm_mul_ps(S1, Q1.vec);
		Result = _mm_add_ps(Result, S1);
		return Result;
	}

	FINLINE Quaternion operator *  (const Quaternion& b) { return Mul(this->vec, b.vec); }
	FINLINE Quaternion operator *= (const Quaternion& b) { this->vec = Mul(this->vec, b.vec); return *this; }
};

FINLINE Quaternion VECTORCALL EulerToQuaternion(Vector3 euler) noexcept
{
	// Abbreviations for the various angular functions
	euler *= 0.5f;

	Vector3 s = _mm_sin_ps(euler.vec);
	Vector3 c = _mm_cos_ps(euler.vec);

	return Quaternion(
		(s.arr[1] * c.arr[2] * c.arr[3]) + (c.arr[1] * s.arr[2] * s.arr[3]),
		(c.arr[1] * s.arr[2] * c.arr[3]) - (s.arr[1] * c.arr[2] * s.arr[3]),
		(c.arr[1] * c.arr[2] * s.arr[3]) + (s.arr[1] * s.arr[2] * c.arr[3]),
		(c.arr[1] * c.arr[2] * c.arr[3]) - (s.arr[1] * s.arr[2] * s.arr[3])
	);
}

inline Vector3 VECTORCALL QuatToEulerAngles(const Quaternion& q) noexcept {
	Vector3 eulerAngles;

	// Threshold for the singularities found at the north/south poles.
	constexpr float SINGULARITY_THRESHOLD = 0.4999995f;

	const float sqw = q.w * q.w;
	const float sqx = q.x * q.x;
	const float sqy = q.y * q.y;
	const float sqz = q.z * q.z;
	const float unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor
	const float singularityTest = (q.x * q.z) + (q.w * q.y);

	if (singularityTest > SINGULARITY_THRESHOLD * unit) {
		eulerAngles.z = 2.0f * atan2(q.x, q.w);
		eulerAngles.y = PI / 2;
		eulerAngles.x = 0;
	}
	else if (singularityTest < -SINGULARITY_THRESHOLD * unit)
	{
		eulerAngles.z = -2.0f * atan2(q.x, q.w);
		eulerAngles.y = -(PI / 2);
		eulerAngles.x = 0;
	}
	else {
		eulerAngles.z = atan2(2 * ((q.w * q.z) - (q.x * q.y)), sqw + sqx - sqy - sqz);
		eulerAngles.y = asin(2 * singularityTest / unit);
		eulerAngles.x = atan2(2 * ((q.w * q.x) - (q.y * q.z)), sqw - sqx - sqy + sqz);
	}
	return eulerAngles;
}

AMATH_END_NAMESPACE