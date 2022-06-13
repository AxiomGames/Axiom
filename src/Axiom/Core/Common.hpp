#pragma once

#include <cstdint>

#ifndef AX_NAMESPACE
#define AX_NAMESPACE namespace ax {
#define AX_END_NAMESPACE }
#endif

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
#	ifndef _MSC_VER
#		define FINLINE  inline
#	else
#		define FINLINE __forceinline
#	endif
#endif

#ifndef ENUM_FLAGS
#define ENUM_FLAGS(enum_name, num_type) \
    inline enum_name operator|(enum_name a, enum_name b)\
    {\
        return static_cast<enum_name>(static_cast<num_type>(a) | static_cast<num_type>(b));\
    }\
    inline bool operator &(enum_name a, enum_name b)\
    {\
        return (static_cast<num_type>(a) & static_cast<num_type>(b)) != 0;\
    }\
    inline enum_name& operator |=(enum_name& a, enum_name b)\
    {\
        return a = a | b;\
    }
#endif

#define ax_assert(...)

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;