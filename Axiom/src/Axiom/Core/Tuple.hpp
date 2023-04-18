#pragma once
// https://hackernoon.com/variadic-template-in-c-implementing-unsophisticated-tuple-w8153ump

template<size_t idx, typename T>
struct GetHelper
{
};

template<class... Ts>
struct Tuple
{
};

template<typename T, typename... Rest>
struct Tuple<T, Rest...>
{
	T first;
	Tuple<Rest...> rest;        // Parameter pack expansion

	Tuple(const T& f, const Rest& ... r) : first(f), rest(r...)
	{}
};

template<
	typename T,
	typename... Rest
>
struct GetHelper<0, Tuple<T, Rest...>>
{
	constexpr static T Get(Tuple<T, Rest...>& data)
	{
		return data.first;
	}
};

template<size_t idx, typename T, typename... Rest>
struct GetHelper<idx, Tuple<T, Rest...>>
{ // GetHelper Implementation
	constexpr static auto Get(Tuple<T, Rest...>& data)
	{
		return GetHelper<idx - 1, Tuple<Rest...>>::Get(data.rest);
	}
};

template<size_t idx, template<typename...> class Tuple, typename... Args>
constexpr inline auto Get(Tuple<Args...>& t)
{
	return GetHelper<idx, Tuple<Args...>>::Get(t);
}