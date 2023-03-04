#pragma once

#include <cstdint>

#if AX_SHARED
#ifdef AX_EXPORT
		#define AX_API __declspec(dllexport)
	#else
		#define AX_API __declspec(dllimport)
	#endif
#else
	#define AX_API
#endif

#ifndef FINLINE
#	ifdef _MSC_VER
#		define FINLINE __forceinline
#   elif __CLANG__
#       define FINLINE [[clang::always_inline]] 
#	elif __GNUC__
#       define FINLINE  inline __attribute__((always_inline))
#   endif
#endif

#ifndef VECTORCALL
#   ifdef _MSC_VER
#		define VECTORCALL __vectorcall
#   elif __CLANG__
#       define VECTORCALL [[clang::vectorcall]] 
#	elif __GNUC__
#       define VECTORCALL  
#   endif
#endif

#if defined( __GNUC__ ) || defined(__INTEGRITY)
#define AX_ALIGNAS(_x)          __attribute__ ((aligned(_x)))
#elif defined( _WIN32) && (_MSC_VER)                                                                                   
#define AX_ALIGNAS(_x)          __declspec(align(_x))      
#else
#warning  Need to implement some method to align data here
#define  AX_ALIGNAS(_x)
#endif

#if _WIN32 || _WIN64
#   define AX_WIN32
#	include <intrin.h>
#elif __linux__
#   define AX_LINUX
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

#define ax_assert(...)

template<typename T>
FINLINE constexpr T PopCount(T x)
{
#ifdef _MSC_VER
	if      constexpr (sizeof(T) == 4) return __popcnt(x);
	else if constexpr (sizeof(T) == 8) return __popcnt64(x);
#elif defined(__GNUC__) && !defined(__MINGW32__)
	if      constexpr (sizeof(T) == 4) return __builtin_popcount(x);
	else if constexpr (sizeof(T) == 8) return __builtin_popcountl(x);
#else
	if constexpr (sizeof(T) == 4)
	{
		i = i - ((i >> 1) & 0x55555555);        // add pairs of bits
		i = (i & 0x33333333) + ((i >> 2) & 0x33333333);  // quads
		i = (i + (i >> 4)) & 0x0F0F0F0F;        // groups of 8
		return (i * 0x01010101) >> 24;          // horizontal sum of bytes	
	}
	else if (sizeof(T) == 8) // standard popcount; from wikipedia
	{
		i -= ((i >> 1) & 0x5555555555555555ull);
		i = (i & 0x3333333333333333ull) + (i >> 2 & 0x3333333333333333ull);
		return ((i + (i >> 4)) & 0xf0f0f0f0f0f0f0full) * 0x101010101010101ull >> 56;
	}
#endif
	ax_assert(0);
}

template<typename T>
FINLINE  constexpr T TrailingZeroCount(T x)
{
#ifdef _MSC_VER
	if      constexpr (sizeof(T) == 4) return _tzcnt_u32(x);
	else if constexpr (sizeof(T) == 8) return _tzcnt_u64(x);
#elif defined(__GNUC__) && !defined(__MINGW32__)
	if      constexpr (sizeof(T) == 4) return __builtin_ctz(x);
	else if constexpr (sizeof(T) == 8) return __builtin_ctzll(x);
#else
	return PopCount((x & -x) - 1);
#endif
	ax_assert(0);
}

template<typename T>
FINLINE  constexpr T LeadingZeroCount(T x)
{
#ifdef _MSC_VER
	if      constexpr (sizeof(T) == 4) return _lzcnt_u32(x);
	else if constexpr (sizeof(T) == 8) return _lzcnt_u64(x);
#elif defined(__GNUC__) && !defined(__MINGW32__)
	if      constexpr (sizeof(T) == 4) return __builtin_clz(x);
	else if constexpr (sizeof(T) == 8) return __builtin_clzll(x);
#else
	x |= (x >> 1);
	x |= (x >> 2);
	x |= (x >> 4);
	x |= (x >> 8);
	x |= (x >> 16);
	return 32 - PopCount(x);
#endif
	ax_assert(0);
}

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

using MemPtr = uint8*;
using VoidPtr = void*;

enum EForceInit
{
	ForceInit
};

