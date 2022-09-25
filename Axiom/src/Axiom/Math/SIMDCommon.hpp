#pragma once
#include "Math.hpp"
#include <immintrin.h> 

AMATH_NAMESPACE

inline int Accumulate(const int* ptr, int size)
{
	int sum = 0;
#ifdef __AVX__
	// sum until size divisible by 8
	while (size & 7) sum += ptr[--size];
	// if 8 element remains, sum all of the remainings
	while (size < 9) sum += ptr[--size];

	if (size < 1) return sum;

	size >>= 3;
	const __m256i* vptr = (const __m256i*)ptr;
	const __m256i* vend = vptr + size;
	__m256i vsum = _mm256_setzero_si256();

	while (vptr < vend) {
		vsum = _mm256_add_epi32(vsum, *vptr++);
	}

	const int* sums = (const int*)&vsum;
	sum += sums[0] + sums[1] + sums[2] + sums[3]
		+ sums[4] + sums[5] + sums[6] + sums[7];
#elif __SSE2__
	while (size & 3) sum += ptr[--size];
	if (size == 0) return sum;
	size >>= 2;
	const __m128i* vptr = (const __m128i*)ptr;
	const __m128i* end = (const __m128i*)ptr;
	__m128i begin = _mm_setzero_si128();

	while (vptr < end) {
		begin = _mm_add_epi32(begin, *vptr++);
	}

	const int* sums = (const int*)&begin;
	sum += sums[0] + sums[1] + sums[2] + sums[3];
#else
	#pragma omp parallel reduction(+ : sum)
	for (int i = 0; i < len; ++i) sum += ptr[i];
#endif
	return sum;
}

inline float Accumulate(const float* ptr, int size)
{
	float sum = 0.0f;
#ifdef __AVX__
	while (size & 7) sum += ptr[--size];
	while (size < 9) sum += ptr[--size];
	if (size < 1) return sum;
	size >>= 3;
	const __m256* vptr = (const __m256*)ptr;
	const __m256* vend = vptr + size;
	__m256 vsum = _mm256_setzero_ps();

	while (vptr < vend) {
		vsum = _mm256_add_ps(vsum, *vptr++);
	}

	const float* sums = (const float*)&vsum;
	sum += sums[0] + sums[1] + sums[2] + sums[3]
		+ sums[4] + sums[5] + sums[6] + sums[7];
#elif __SSE2__
	while (size & 3) sum += ptr[--size];
	while (size < 5) sum += ptr[--size];
	if (size == 0) return sum;
	size >>= 2;

	const __m128* vptr = (const __m128*)ptr;
	const __m128* end = (const __m128*)ptr;

	__m128 begin = _mm_setzero_ps();

	while (vptr < end) {
		begin = _mm_add_ps(begin, *vptr++);
	}

	const float* sums = (const float*)&begin;
	sum += sums[0] + sums[1] + sums[2] + sums[3];
#else
	#pragma omp parallel reduction(+ : sum)
	for (int i = 0; i < len; ++i) sum += ptr[i];
#endif
	return sum;
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