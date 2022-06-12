#pragma once

#include "Axiom/Core/Array.hpp"
#include "Axiom/Core/robin_hood.h"
#include "Axiom/Framework/Object.hpp"

namespace Ax
{
typedef uint32_t EntityID;

struct ESCRegistry
{


	EntityID NewEntity()
	{
		return 0;
	}

	bool RemoveEntity(EntityID entity)
	{
		return false;
	}

	template<typename T, typename ... Args>
	T* AddComponent(EntityID entity, Args... args)
	{
		return nullptr;
	}

	template<typename T>
	T* RemoveComponent()
	{
		return nullptr;
	}

	template<typename T>
	T* GetComponent()
	{
		return nullptr;
	}

	template<typename T>
	Array<T*> GetComponents()
	{
		Array<T*> array;

		return array;
	}
};
}