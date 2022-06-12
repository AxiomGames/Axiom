#pragma once

namespace Ax
{
template<typename Key, typename Val>
struct Pair
{
	Key key;
	Val value;

	bool operator<(const Pair& other) const
	{
		if (key > other.key) return false;
		if (other.key > key) return false;

		if (value > other.value) return false;
		if (other.value > value) return false;

		return true;
	}
};
}