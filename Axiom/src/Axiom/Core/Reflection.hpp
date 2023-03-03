#pragma once

#include "TemplateBase.hpp"
#include "UnorderedMap.hpp"
#include "StringView.hpp"
#include "SharedPtr.hpp"
#include "Array.hpp"
#include "Span.hpp"
#include "TypeID.hpp"

struct AttributeStorage
{
	ConstVoidPtr (* GetValue)();
};

struct FunctionStorage
{
	String Name{};
	VoidPtr InvokeAllParams{};

	void (* InvokeArray)(VoidPtr instance, VoidPtr ret, VoidPtr* params);
};

struct ValueStorage
{
	String Name{};

	ConstVoidPtr (* GetValue)();

	UnorderedMap<TTypeID, SharedPtr<AttributeStorage>> Attributes;
};

struct TypeStorage
{
	String Name{};
	UnorderedMap<std::size_t, SharedPtr<FunctionStorage>> FunctionsByName{};
	UnorderedMap<std::size_t, SharedPtr<ValueStorage>> ValuesByName{};
	UnorderedMap<std::size_t, ValueStorage*> ValuesByHash{};
	Array<ValueStorage*> ValuesByOrder{};
};

template<typename Owner, typename AttrType>
class NativeAttributeHandler
{
public:
	inline static AttrType AttrValue{};

	explicit NativeAttributeHandler(AttributeStorage& attributeStorage)
	{
		attributeStorage.GetValue = GetValueImpl;
	}

private:
	static ConstVoidPtr GetValueImpl()
	{
		return &AttrValue;
	}
};


template<auto Value, typename Type, typename Owner>
class NativeValueHandler
{
public:
	using ValueType = NativeValueHandler<Value, Type, Owner>;

	explicit NativeValueHandler(ValueStorage& valueStorage) : m_ValueStorage(valueStorage)
	{
		m_ValueStorage.GetValue = GetValueImpl;
	}

	template<typename AttrType>
	decltype(auto) Attribute(AttrType&& attr)
	{
		NativeAttributeHandler<ValueType, AttrType>::AttrValue = std::forward<AttrType>(attr);
		auto typeId = TypeIDCache<AttrType>::Value;
		auto it = m_ValueStorage.Attributes.Find(typeId);
		if (it == m_ValueStorage.Attributes.end())
		{
			it = m_ValueStorage.Attributes.Insert(MakePair(typeId, MakeShared<AttributeStorage>())).first;
			NativeAttributeHandler<ValueType, AttrType>{*it->second};
		}
		return *this;
	}

private:
	static constexpr Type c_Value = Value;

	ValueStorage& m_ValueStorage;

	static ConstVoidPtr GetValueImpl()
	{
		return &c_Value;
	}
};

template<auto MFP, typename Return, typename Owner, typename ...Args>
class NativeMemberFunctionHandler
{
public:
	explicit NativeMemberFunctionHandler(FunctionStorage& functionStorage) : m_FunctionStorage(functionStorage)
	{
		m_FunctionStorage.InvokeAllParams = reinterpret_cast<VoidPtr>(&InvokeAllParams);
		m_FunctionStorage.InvokeArray = &InvokeArray;
	}

private:

	static void InvokeAllParams(VoidPtr instance, VoidPtr ret, TRemoveRef<Args>* ...args)
	{
		*static_cast<TRemoveRef<Return>*>(ret) = (static_cast<Owner*>(instance)->*MFP)(*static_cast<TRemoveRef<Args>*>(args)...);
	}

	static void InvokeArray(VoidPtr instance, VoidPtr ret, VoidPtr* params)
	{
		std::size_t i{sizeof...(Args)};
		*static_cast<TRemoveRef<Return>*>(ret) = (static_cast<Owner*>(instance)->*MFP)(*static_cast<TRemoveRef<Args>*>(params[--i])...);
	}

	FunctionStorage& m_FunctionStorage;
};


template<auto MFP, typename Function>
struct MemberFunctionTemplateDecomposer
{
};

template<auto MFP, typename Return, typename Owner, typename ...Args>
struct MemberFunctionTemplateDecomposer<MFP, Return(Owner::*)(Args...)>
{
	[[maybe_unused]] static auto CreateHandler(FunctionStorage& functionStorage)
	{
		return NativeMemberFunctionHandler<MFP, Return, Owner, Args...>(functionStorage);
	}
};

template<auto MFP, typename Return, typename Owner, typename ...Args>
struct MemberFunctionTemplateDecomposer<MFP, Return(Owner::*)(Args...) const>
{
	[[maybe_unused]] static auto CreateHandler(FunctionStorage& functionStorage)
	{
		return NativeMemberFunctionHandler<MFP, Return, Owner, Args...>(functionStorage);
	}
};


template<typename T>
class NativeTypeHandler
{
public:
	explicit NativeTypeHandler(TypeStorage& typeStorage) : m_TypeStorage(typeStorage)
	{

	}

	template<auto MFP>
	auto NewFunction(const StringView& functionName)
	{
		auto hashName = HashValue(functionName);
		auto it = m_TypeStorage.FunctionsByName.Find(hashName);
		if (it == m_TypeStorage.FunctionsByName.end())
		{
			it = m_TypeStorage.FunctionsByName.Insert(Pair<std::size_t,
				SharedPtr<FunctionStorage>>(hashName, MakeShared<FunctionStorage>())
			).first;
		}
		return MemberFunctionTemplateDecomposer<MFP, decltype(MFP)>::CreateHandler(*it->second);
	}

	template<auto Value>
	auto Value(const StringView& valueName)
	{
		auto hashName = HashValue(valueName);
		auto it = m_TypeStorage.ValuesByName.Find(hashName);
		if (it == m_TypeStorage.ValuesByName.end())
		{
			it = m_TypeStorage.ValuesByName.Insert(MakePair(hashName, MakeShared<ValueStorage>(
				String{valueName.begin(), valueName.end()}
			))).first;

			m_TypeStorage.ValuesByHash.Insert(MakePair(HashValue(Value), it->second.get()));
			m_TypeStorage.ValuesByOrder.PushBack(it->second.get());
		}
		return NativeValueHandler<Value, decltype(Value), T>{*it->second};
	}

private:
	TypeStorage& m_TypeStorage;
};

//---handlers

class ValueHandler
{
public:
	explicit ValueHandler(ValueStorage* valueStorage) : m_ValueStorage(valueStorage)
	{}

	explicit operator bool()
	{
		return m_ValueStorage != nullptr;
	}

	template<typename Type>
	const Type& As() const
	{
		return *static_cast<const Type*>(m_ValueStorage->GetValue());
	}

	[[nodiscard]] StringView GetName() const
	{
		return m_ValueStorage->Name;
	}

	bool HasAttribute(TTypeID typeId)
	{
		return m_ValueStorage->Attributes.Find(typeId) != m_ValueStorage->Attributes.end();
	}

	template<typename Type>
	bool HasAttribute()
	{
		return HasAttribute(TypeIDCache<Type>::Value);
	}

	ConstVoidPtr GetAttribute(TTypeID typeId)
	{
		auto it = m_ValueStorage->Attributes.Find(typeId);
		if (it != m_ValueStorage->Attributes.end())
		{
			return it->second->GetValue();
		}
		return nullptr;
	}

	template<typename Type>
	decltype(auto) GetAttribute()
	{
		return *static_cast<const Type*>(GetAttribute(TypeIDCache<Type>::Value));
	}

private:
	ValueStorage* m_ValueStorage{};
};


class FunctionHandler
{
public:
	explicit FunctionHandler(FunctionStorage* mFunctionStorage) : m_FunctionStorage(mFunctionStorage)
	{}

	explicit operator bool()
	{
		return m_FunctionStorage != nullptr;
	}

	template<typename Ret, typename ...Args>
	Ret Invoke(VoidPtr instance, Args&& ...args)
	{
		Ret ret{};
		VoidPtr params[] = {&args...,};
		m_FunctionStorage->InvokeArray(instance, &ret, params);
		return ret;
	}

private:
	FunctionStorage* m_FunctionStorage;
};

class TypeHandler
{
public:
	explicit TypeHandler(TypeStorage* typeStorage) : m_TypeStorage(typeStorage)
	{
	}

	explicit operator bool()
	{
		return m_TypeStorage != nullptr;
	}

	auto FindFunction(const StringView& functionName)
	{
		auto hashName = HashValue(functionName);
		auto it = m_TypeStorage->FunctionsByName.Find(hashName);
		if (it != m_TypeStorage->FunctionsByName.end())
		{
			return FunctionHandler{it->second.get()};
		}
		return FunctionHandler{nullptr};
	}

	auto FindValueByName(const StringView& valueName)
	{
		auto it = m_TypeStorage->ValuesByName.Find(HashValue(valueName));
		if (it != m_TypeStorage->ValuesByName.end())
		{
			return ValueHandler{it->second.get()};
		}
		return ValueHandler{nullptr};
	}

	template<typename Type>
	auto FindValue(const Type& value)
	{
		auto it = m_TypeStorage->ValuesByHash.Find(HashValue(value));
		if (it != m_TypeStorage->ValuesByHash.end())
		{
			return ValueHandler{it->second};
		}
		return ValueHandler{nullptr};
	}

	[[nodiscard]] auto Values() const
	{
		return Span<ValueHandler>(
			reinterpret_cast<ValueHandler*>(m_TypeStorage->ValuesByOrder.begin()),
			reinterpret_cast<ValueHandler*>(m_TypeStorage->ValuesByOrder.end()));
	}


private:
	TypeStorage* m_TypeStorage{};
};

class AX_API Reflection
{
private:
	UnorderedMap<Hash_t, SharedPtr<TypeStorage>> m_TypesByName;

	Reflection() = default;
public:
	template<typename T>
	auto NewType(const StringView& typeName)
	{
		auto hashName = HashValue(typeName);
		auto it = m_TypesByName.Find(hashName);
		if (it == m_TypesByName.end())
		{
			it = m_TypesByName.Insert(Pair<Hash_t, SharedPtr<TypeStorage>>(hashName, MakeShared<TypeStorage>(
				String{typeName.begin(), typeName.end()}
			))).first;
		}
		return NativeTypeHandler<T>{*it->second};
	}

	auto FindType(const StringView& typeName)
	{
		auto hashName = HashValue(typeName);
		auto it = m_TypesByName.Find(hashName);
		if (it != m_TypesByName.end())
		{
			return TypeHandler{it->second.get()};
		}
		return TypeHandler{nullptr};
	}

	auto FindType(Hash_t hash)
	{
		auto it = m_TypesByName.Find(hash);
		if (it != m_TypesByName.end())
		{
			return TypeHandler{it->second.get()};
		}
		return TypeHandler{nullptr};
	}

	static Reflection& Get()
	{
		static Reflection reflection;
		return reflection;
	}
};

struct CompileStaticRegister
{
	template<typename Fnc>
	CompileStaticRegister(Fnc fnc)
	{
		fnc(Reflection::Get());
	}
};

template<typename T>
TypeHandler StaticType();
