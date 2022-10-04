#pragma once

#include "Core/TypeID.hpp"
#include <unordered_map>

class GlobalEngineObjects
{
private:
	std::unordered_map<TTypeID, void*> m_Objects;
public:
	template<class T>
	bool IsInitialized()
	{
		return m_Objects.contains(TypeIDCache<T>::Value);
	}

	template<class T, typename... Args>
	T* Initialize(Args&&... args)
	{
		if (IsInitialized<T>())
			return nullptr;

		T* instance = new T(std::forward<Args>(args)...);
		m_Objects[TypeIDCache<T>::Value] = instance;
		return instance;
	}

	template<class T>
	bool Destroy()
	{
		auto foundIt = m_Objects.find(TypeIDCache<T>::Value);

		if (foundIt == m_Objects.end())
		{
			return false;
		}

		T* obj = (T*)foundIt->second;
		m_Objects.erase(foundIt);
		delete obj;

		return true;
	}

	template<class T>
	T* Get()
	{
		auto foundIt = m_Objects.find(TypeIDCache<T>::Value);

		if (foundIt == m_Objects.end())
		{
			return nullptr;
		}

		return (T*)foundIt->second;
	}
};

extern AX_API GlobalEngineObjects* GEngine;
