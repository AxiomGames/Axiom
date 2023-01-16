#pragma once

#include "Common.hpp"
#include "Traits.hpp"

//template<typename TKey, typename TVal>
//struct Pair
//{
//	TKey Key;
//	TVal Value;
//
//	bool operator == (const Pair& other)
//	{
//		return Key == other.Key && Value == other.Value;
//	}
//
//	bool operator != (const Pair& other)
//	{
//		return Key != other.Key || Value != other.Value;
//	}
//
//	bool operator<(const Pair& other) const
//	{
//		if (Key > other.Key) return false;
//		if (other.Key > Key) return false;
//
//		if (Value > other.Value) return false;
//		if (other.Value > Value) return false;
//
//		return true;
//	}
//};


template<typename TKey, typename TValue>
struct Pair
{
	Pair();

	Pair(const Pair& other);

	Pair(Pair&& other);

	Pair(const TKey& key, const TValue& value);

	Pair(TKey&& key, TValue&& value);

	Pair& operator=(const Pair& other);

	Pair& operator=(Pair&& other);

	bool operator<(const Pair& other) const;

	TKey first;
	TValue second;
};

template<typename Key, typename Value>
inline Pair<Key, Value>::Pair()
{
}

template<typename Key, typename Value>
inline Pair<Key, Value>::Pair(const Pair& other)
	: first(other.first), second(other.second)
{
}

template<typename Key, typename Value>
inline Pair<Key, Value>::Pair(Pair&& other)
	: first(static_cast<Key&&>(other.first)), second(static_cast<Value&&>(other.second))
{
}

template<typename Key, typename Value>
inline Pair<Key, Value>::Pair(const Key& key, const Value& value)
	: first(key), second(value)
{
}

template<typename Key, typename Value>
inline Pair<Key, Value>::Pair(Key&& key, Value&& value)
	: first(static_cast<Key&&>(key)), second(static_cast<Value&&>(value))
{
}

template<typename Key, typename Value>
inline Pair<Key, Value>& Pair<Key, Value>::operator=(const Pair& other)
{
	first = other.first;
	second = other.second;
	return *this;
}

template<typename Key, typename Value>
inline Pair<Key, Value>& Pair<Key, Value>::operator=(Pair&& other)
{
	first = static_cast<Key&&>(other.first);
	second = static_cast<Value&&>(other.second);
	return *this;
}

template<typename TKey, typename TValue>
bool Pair<TKey, TValue>::operator<(const Pair& other) const
{
	if (first > other.first) return false;
	if (other.first > first) return false;

	if (second > other.Value) return false;
	if (other.Value > second) return false;

	return true;
}

template<typename Key, typename Value>
static inline Pair<typename Traits::remove_reference<Key>::type, typename Traits::remove_reference<Value>::type> MakePair(Key&& key, Value&& value)
{
	return Pair<typename Traits::remove_reference<Key>::type, typename Traits::remove_reference<Value>::type>(
		static_cast<Key&&>(key), static_cast<Value&&>(value)
	);
}