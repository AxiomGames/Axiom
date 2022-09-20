#pragma once

#include "Common.hpp"

AX_NAMESPACE

template<typename TKey, typename TVal>
struct Pair
{
	TKey Key;
	TVal Value;

	bool operator == (const Pair& other)
	{
		return Key == other.Key && Value == other.Value;
	}

	bool operator != (const Pair& other)
	{
		return Key != other.Key || Value != other.Value;
	}

	bool operator<(const Pair& other) const
	{
		if (Key > other.Key) return false;
		if (other.Key > Key) return false;

		if (Value > other.Value) return false;
		if (other.Value > Value) return false;

		return true;
	}
};

AX_END_NAMESPACE