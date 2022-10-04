#pragma once

#include "String.hpp"

#ifndef AX_PRETTY_FUNCTION
#   if defined _MSC_VER
#      define AX_PRETTY_FUNCTION __FUNCSIG__
#   else
#       if defined __GNUC__
#           define AX_PRETTY_FUNCTION __PRETTY_FUNCTION__
#       else
static_assert(false, "OS still not supported");
#       endif
#   endif
#endif

typedef uint64_t TTypeID;

template<typename Type>
struct TypeIDCache
{
	constexpr static const char* GetHashName() { return AX_PRETTY_FUNCTION; }
	constexpr static const uint64 Value = HashDjb2(GetHashName());
};
