#pragma once

#include "Common.hpp"
#include <cstring>

template<typename T>
struct RemoveRef { typedef T Type; };

template<typename T>
struct RemoveRef<T&> { typedef T Type; };

template<typename T>
struct RemoveRef<T&&> { typedef T Type; };

template<typename T>
struct RemovePtr { typedef T Type; };

template<typename T>
struct RemovePtr<T*> { typedef T Type; };

template<typename T>
FINLINE typename RemoveRef<T>::Type&& Move(T&& obj)
{
	typedef typename RemoveRef<T>::Type CastType;
	return (CastType&&)obj;
}

template<typename T>
FINLINE T&& Forward(typename RemoveRef<T>::Type& obj)
{
	return (T&&)obj;
}

template<typename T>
FINLINE T&& Forward(typename RemoveRef<T>::Type&& obj)
{
	return (T&&)obj;
}

template<typename T>
FINLINE void SwapPrimitive(T& left, T& right)
{
	T temp = Move(left);
	left = Move(right);
	right = Move(temp);
}

template<class T, class U = T>
FINLINE constexpr T Exchange(T& obj, U&& new_value)
{
  T old_value = Move(obj);
  obj = Forward<U>(new_value);
  return old_value;
}

template<typename T>
FINLINE void SwapMemory(T& left, T& right)
{
	uint8_t temp[sizeof(T)];
	memcpy(temp, &left, sizeof(T));
	memcpy(&left, &right, sizeof(T));
	memcpy(&right, temp, sizeof(T));
}

template<typename T, typename ArgType>
FINLINE T StaticCast(ArgType&& arg)
{
	return static_cast<T>(arg);
}

template<typename T, typename ArgType>
FINLINE T DynamicCast(ArgType&& arg)
{
	return dynamic_cast<T>(arg);
}
