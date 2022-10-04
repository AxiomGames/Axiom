#pragma once

#include <atomic>
#include <type_traits>
#include <memory>

#include "../Core/Array.hpp"
#include "../Core/robin_hood.h"
#include "../Core/CMBAllocator.hpp"
#include "Object.hpp"

typedef uint32_t EntityID;

struct ComponentBase : ObjectBase
{
	CLASS_OBJ(ComponentBase, ObjectBase);
};

template <typename T>
concept ComponentType = std::is_base_of<ComponentBase, T>::value;

template<typename T>
class ComponentIterator
{
private:
	T** m_Current;
	size_t m_Index;
public:
	explicit ComponentIterator(T** current, size_t index) : m_Current(current), m_Index(index) {}

	T* operator*()
	{
		return *m_Current;
	}

	ComponentIterator& operator++()
	{
		m_Current++;
		m_Index++;

		return *this;
	}

	bool operator==(const ComponentIterator<T>& other) const
	{
		return m_Index == other.m_Index;
	}

	bool operator!=(const ComponentIterator<T>& other) const
	{
		return m_Index != other.m_Index;
	}
};

template<typename T>
class ComponentView
{
private:
	Array<std::uintptr_t> m_DataVector;
public:
	ComponentView() : m_DataVector() {}

	explicit ComponentView(Array<std::uintptr_t>& data) : m_DataVector(std::move(data))
	{
	}

	[[nodiscard]] std::vector<std::uintptr_t>::size_type size() const { return m_DataVector.Size(); }
	[[nodiscard]] bool empty() const { return m_DataVector.Empty(); }

	ComponentIterator<T> begin()
	{
		if(m_DataVector.Empty())
		{
			return ComponentIterator<T>(nullptr, 0);
		}

		return ComponentIterator<T>(reinterpret_cast<T**>(&m_DataVector[0]), 0);
	}

	ComponentIterator<T> end()
	{
		if(m_DataVector.Empty())
		{
			return ComponentIterator<T>(nullptr, 0);
		}

		return ComponentIterator<T>(nullptr, m_DataVector.Size());
	}
};

class ECSRegistry
{
private:
	struct EntityData
	{
		robin_hood::unordered_map<TTypeID, std::uintptr_t> ComponentPointers;
		//robin_hood::unordered_map<TTypeID, std::uintptr_t> ComponentOffsets; // TODO: This will be for serialization
	};

	robin_hood::unordered_map<TTypeID, CMBAllocator*> m_ComponentMemory;
	robin_hood::unordered_map<TTypeID, Array<std::uintptr_t>> m_ComponentPointers;
	robin_hood::unordered_map<EntityID, EntityData> m_Entities;

	std::atomic<EntityID> m_EntityCounter{0};
public:
	~ECSRegistry()
	{
		for (const auto& [typeID, Allocator] : m_ComponentMemory)
		{
			//delete Allocator;
		}
	}

	FINLINE EntityID NewEntity()
	{
		EntityID newEntity = m_EntityCounter.fetch_add(1);
		m_Entities[newEntity] = {};
		return newEntity;
	}

	bool RemoveEntity(EntityID entity)
	{
		const auto& it = m_Entities.find(entity);

		if (it == m_Entities.end())
		{
			return false;
		}

		for (const auto& [typeID, CmpPointer] : it->second.ComponentPointers)
		{
			DestroyComponent<ComponentBase>(reinterpret_cast<ComponentBase*>(CmpPointer));
		}

		m_Entities.erase(it);
		return true;
	}

	template<ComponentType T, typename ... Args>
	FINLINE T* AddComponent(EntityID entity, Args... args)
	{
		const auto& it = m_Entities.find(entity);

		if (it == m_Entities.end())
		{
			ax_assert(0);
			return nullptr;
		}

		TTypeID componentID = T::TypeID();

		EntityData& entityData = it->second;

		if (entityData.ComponentPointers.contains(componentID))
		{
			// Cannot add another component of same type
			ax_assert(0);
			return nullptr;
		}

		T* component = AllocComponent<T>(std::forward<Args>(args)...);

		entityData.ComponentPointers[componentID] = (std::uintptr_t)component;

		return component;
	}

	template<ComponentType T>
	bool RemoveComponent(EntityID entity)
	{
		// TODO: Think if remove should return copy of the component

		const auto& it = m_Entities.find(entity);

		if (it == m_Entities.end())
		{
			return false;
		}

		EntityData& entityData = it->second;

		TTypeID componentID = T::TypeID();

		const auto& cmpIt = entityData.ComponentPointers.find(componentID);

		if (cmpIt == entityData.ComponentPointers.end())
		{
			return false;
		}

		T* component = reinterpret_cast<T*>(cmpIt->second);
		DestroyComponent(component);
		entityData.ComponentPointers.erase(cmpIt);

		return true;
	}

	template<ComponentType T>
	T* GetComponent(EntityID entity)
	{
		const auto& it = m_Entities.find(entity);

		if (it == m_Entities.end())
		{
			return nullptr;
		}

		EntityData& entityData = it->second;

		TTypeID componentID = T::TypeID();

		const auto& cmpIt = entityData.ComponentPointers.find(componentID);

		if (cmpIt == entityData.ComponentPointers.end())
		{
			return nullptr;
		}

		return reinterpret_cast<T*>(cmpIt->second);
	}

	template<ComponentType T>
	FINLINE bool HasComponent(EntityID entity)
	{
		const auto& it = m_Entities.find(entity);

		if (it == m_Entities.end())
		{
			return false;
		}

		return it->second.ComponentPointers.contains(T::TypeID());
	}

	template<ComponentType T>
	ComponentView<T> GetComponents()
	{
		Array<std::uintptr_t> foundComponents;

		for(auto& it : m_ComponentPointers)
		{
			if(it.second.Empty()) continue;

			auto* typeBase = reinterpret_cast<ObjectBase*>(it.second[0]);

			if(typeBase->HasType(T::TypeID()))
			{
				foundComponents.Insert(it.second);
			}
		}

		return ComponentView<T>(foundComponents);
	}

	template<ComponentType T>
	void IterateComponents(void(*IterateFunc)(T*))
	{
		Array<std::uintptr_t> foundComponents;

		for (auto& it : m_ComponentPointers)
		{
			if (it.second.Empty()) continue;

			auto* typeBase = reinterpret_cast<ObjectBase*>(it.second[0]);

			if (typeBase->HasType(T::TypeID()))
			{
				for (std::uintptr_t ptr : it.second)
				{
					IterateFunc(reinterpret_cast<T*>(ptr));
				}
			}
		}
	}

private:
	template<ComponentType T, typename ... Args>
	FINLINE T* AllocComponent(Args... args)
	{
		TTypeID componentID = T::TypeID();

		CMBAllocator* allocator = nullptr;
		auto it = m_ComponentMemory.find(componentID);

		if(it != m_ComponentMemory.end())
		{
			allocator = it->second;
		}
		else
		{
			allocator = new CMBAllocator(8388608);
			//allocator->SetName(std::string("ComponentMemory:") + T::TypeName());
			m_ComponentMemory.emplace(componentID, allocator);
			//AU_LOG_INFO("New allocator for component ", T::TypeName(), " with size of ", FormatBytes(componentSize), " aligned ", FormatBytes(componentSizeAligned));
		}

		T* component = allocator->AllocAligned<T>(16u, std::forward<Args>(args)...);
		m_ComponentPointers[componentID].Add((std::uintptr_t)component);
		return component;
	}

	template<ComponentType T>
	void DestroyComponent(T* component)
	{
		TTypeID componentID = component->GetTypeID();

		auto it = m_ComponentMemory.find(componentID);
		CMBAllocator* allocator = it->second;
		allocator->Free<T>(component);
		m_ComponentPointers[componentID].Remove((uintptr_t)component);
	}
};
