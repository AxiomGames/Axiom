#pragma once

#include "String.hpp"
#include "StringView.hpp"

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
	static constexpr StringView ExtractTypeName(const char* name)
	{
		std::string_view view(name);

		auto first = view.find('<');
		auto last = view.find_last_of('>');

		std::string_view result = view.substr(first + 1, last - first - 1);
		if (result[result.length() - 1] == ' ')
		{
			result = result.substr(0, result.length() - 1);
		}

		return {result.data(), result.length()};
	}

	constexpr static const char* GetHashNameRaw()
	{ return AX_PRETTY_FUNCTION; }

	constexpr static StringView GetHashTypeName()
	{ return ExtractTypeName(GetHashNameRaw()); }

	constexpr static const uint64 Value = HashValue(GetHashTypeName());
};
