/*-
 * this file is based on: https://github.com/mendsley/tinystl
 *
 * Copyright 2012-2018 Matthew Endsley
 * All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted providing that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "Common.hpp"

template<typename T>
struct Hash
{
	constexpr static bool c_HasHashImpl = true;

	static size_t hash(const T& value)
	{
		const auto asint = (std::size_t) value;
		const auto str = (const char*) &asint;
		const auto len = sizeof(asint);

		size_t hash = 0;
		typedef const char* pointer;
		for (pointer it = str, end = str + len; it != end; ++it)
		{
			hash = *it + (hash << 6) + (hash << 16) - hash;
		}

		return hash;
	}
};

template<typename TValue>
inline size_t HashValue(const TValue& value)
{
	static_assert(Hash<TValue>::c_HasHashImpl, "type has no hash implementation");
	return Hash<TValue>::hash(value);
}

// https://github.com/martinus/unordered_dense/blob/main/include/ankerl/unordered_dense.h
// This is a stripped-down implementation of wyhash: https://github.com/wangyi-fudan/wyhash
// No big-endian support (because different values on different machines don't matter),
// hardcodes seed and the secret, reformattes the code, and clang-tidy fixes.
namespace WYHash
{
	FINLINE void mum(uint64_t* RESTRICT a, uint64_t* RESTRICT b)
	{
#if defined(__SIZEOF_INT128__)
		__uint128_t r = *a;
		r *= *b;
		*a = static_cast<uint64_t>(r);
		*b = static_cast<uint64_t>(r >> 64U);
#elif defined(_MSC_VER) && defined(_M_X64)
		*a = _umul128(*a, *b, b);
#else
		uint64_t ha = *a >> 32U;
		uint64_t hb = *b >> 32U;
		uint64_t la = static_cast<uint32_t>(*a);
		uint64_t lb = static_cast<uint32_t>(*b);
		uint64_t hi{};
		uint64_t lo{};
		uint64_t rh = ha * hb;
		uint64_t rm0 = ha * lb;
		uint64_t rm1 = hb * la;
		uint64_t rl = la * lb;
		uint64_t t = rl + (rm0 << 32U);
		auto c = static_cast<uint64_t>(t < rl);
		lo = t + (rm1 << 32U);
		c += static_cast<uint64_t>(lo < t);
		hi = rh + (rm0 >> 32U) + (rm1 >> 32U) + c;
		*a = lo;
		*b = hi;
#endif
	}

	FINLINE uint64 mix(uint64 a, uint64 b) { mum(&a, &b); return a ^ b; }
	FINLINE uint64 r8(const uint8* p) { return *(uint64*)p; }
	FINLINE uint64 r4(const uint8* p) { return (uint64) * (uint32*)p; }
	// reads 1, 2, or 3 bytes
	FINLINE uint64 r3(const uint8* p, size_t k)
	{
		return (uint64_t(p[0]) << 16U) | (uint64_t(p[k >> 1U]) << 8U) | p[k - 1];
	}

	[[nodiscard]] FINLINE uint64 Hash(void const* key, size_t len)
	{
		const size_t secret[4] = { 0xa0761d6478bd642full,
		0xe7037ed1a0b428dbull,
		0x8ebc6af09c88c6e3ull,
		0x589965cc75374cc3ull };

		uint8 const* p = (uint8 const*)key;
		uint64_t seed = secret[0];
		uint64_t a{}, b{};

		if (AX_LIKELY(len <= 16)) {
			if (AX_LIKELY(len >= 4)) {
				a = (r4(p) << 32U) | r4(p + ((len >> 3U) << 2U));
				b = (r4(p + len - 4) << 32U) | r4(p + len - 4 - ((len >> 3U) << 2U));
			}
			else if (AX_LIKELY(len > 0)) {
				a = r3(p, len);
				b = 0;
			}
			else {
				a = 0;
				b = 0;
			}
		}
		else {
			size_t i = len;
			if (AX_UNLIKELY(i > 48)) {
				uint64_t see1 = seed;
				uint64_t see2 = seed;
				do {
					seed = mix(r8(p) ^ secret[1], r8(p + 8) ^ seed);
					see1 = mix(r8(p + 16) ^ secret[2], r8(p + 24) ^ see1);
					see2 = mix(r8(p + 32) ^ secret[3], r8(p + 40) ^ see2);
					p += 48;
					i -= 48;
				} while (AX_LIKELY(i > 48));
				seed ^= see1 ^ see2;
			}
			while (AX_UNLIKELY(i > 16)) {
				seed = mix(r8(p) ^ secret[1], r8(p + 8) ^ seed);
				i -= 16;
				p += 16;
			}
			a = r8(p + i - 16);
			b = r8(p + i - 8);
		}

		return mix(secret[1] ^ len, mix(a ^ secret[1], b ^ seed));
	}

	[[nodiscard]] FINLINE uint64 Hash(uint64 x)
	{
		return mix(x, UINT64_C(0x9E3779B97F4A7C15));
	}
}

FINLINE uint32 FNVHash(const void* ptr, uint64 length)
{
	const char* str = (const char*)ptr;
	uint32 hash = 2166136261u;

	for (uint32 i = 0; i < length; str++, i++)
	{
		hash *= 16777619u;
		hash ^= *str;
	}
	return hash;
}

FINLINE uint64 FNVHash64(const void* ptr, uint64 length)
{
	const char* str = (const char*)ptr;
	uint64 hash = 14695981039346656037ull;

	for (uint64 i = 0; i < length; str++, i++)
	{
		hash *= 1099511628211ULL;
		hash ^= *str;
	}
	return hash;
}

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

template<typename T> struct  Hasher
{
	static FINLINE uint64 Hash(const T& x)
	{
		if constexpr (sizeof(T) == 4) return uint64(WangHash(x)) * 0x9ddfea08eb382d69ull;
		else if constexpr (sizeof(T) == 8) return MurmurHash(x);
		else return WYHash::Hash(&x, sizeof(T));
	}
};