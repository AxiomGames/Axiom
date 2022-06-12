#pragma once
#include "Math.hpp"
#include "Matrix4.h"
#include "Vector3.hpp"
#include "Vector4.hpp"

AMATH_NAMESPACE

struct Matrix4d
{
	union
	{
		__m256d  r[4];
		Vector4d  vec[4];
		struct { __m256d  r1, r2, r3, r4; };
		struct { Vector4d vec1, vec2, vec3, vec4; };
		float m[4][4];
	};

	Matrix4d()
	:	r1(_mm256_set_pd(1, 0, 0, 0)), 
		r2(_mm256_set_pd(0, 1, 0, 0)), 
		r3(_mm256_set_pd(0, 0, 1, 0)), 
		r4(_mm256_set_pd(0, 0, 0, 1)) { }
	
	// maybe fill this but working with 1024 bit struct is a bit unnecessary

	VECTORCALL Matrix4d(const __m256d x, const __m256d y, const __m256d& z, const __m256d& w) : r1(x), r2(y), r3(z), r4(w) {}
	VECTORCALL Matrix4d(const Vector4d x, const Vector4d y, const Vector4d& z, const Vector4d& w) : vec1(x), vec2(y), vec3(z), vec4(w) {}
};

FINLINE Matrix4 MatrixDoubleToFloat(const Matrix4d& md)
{
	return Matrix4(
		_mm256_cvtpd_ps(md.r1),
		_mm256_cvtpd_ps(md.r1),
		_mm256_cvtpd_ps(md.r1),
		_mm256_cvtpd_ps(md.r1)
	);
}

FINLINE Matrix4d MatrixFloatToDouble(const Matrix4& mf)
{
	return Matrix4d(
		_mm256_cvtps_pd(mf.r1),
		_mm256_cvtps_pd(mf.r1),
		_mm256_cvtps_pd(mf.r1),
		_mm256_cvtps_pd(mf.r1)
	);
}

AMATH_END_NAMESPACE