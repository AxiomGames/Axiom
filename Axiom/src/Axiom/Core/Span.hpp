#pragma once

#include "Common.hpp"

template<typename T, typename Alloc>
class Array;

template<typename T>
class Span
{
public:
	typedef T ValueType;
	typedef T* Iterator;
	typedef const T* ConstIterator;


	Span() : m_First(0), m_Last(0)
	{}

	template<class Alloc>
	Span(Array<T, Alloc>& arr) : m_First(arr.begin()), m_Last(arr.end())
	{
	}

	Span(T* mFirst, T* mLast) : m_First(mFirst), m_Last(mLast)
	{}

	constexpr const T* Data() const
	{
		return begin();
	}

	constexpr Iterator begin()
	{
		return m_First;
	}

	constexpr Iterator end()
	{
		return m_Last;
	}

	constexpr ConstIterator begin() const
	{
		return m_First;
	}

	constexpr ConstIterator end() const
	{
		return m_Last;
	}

	[[nodiscard]] constexpr size_t Size() const
	{
		return (size_t) (end() - begin());
	}

	[[nodiscard]] bool Empty() const
	{
		return Size() == 0;
	}

	T& operator[](size_t idx)
	{
		return begin()[idx];
	}

	const T& operator[](size_t idx) const
	{
		return begin()[idx];
	}

	constexpr const T& Back() const
	{
		return begin()[Size() - 1];
	}

	constexpr T& Back()
	{
		return begin()[Size() - 1];
	}

private:
	T* m_First;
	T* m_Last;
};

