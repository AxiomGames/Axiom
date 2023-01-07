#pragma once

#include <cstdint>
#include <memory>
#include <filesystem>

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
#       define FINLINE  __attribute__((always_inline))
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

#define ax_assert(...)

using Path = std::filesystem::path;
using FileStream = std::fstream;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

enum EForceInit
{
	ForceInit
};