#pragma once

#include "UnorderedMap.hpp"
#include "StringView.hpp"
#include "SharedPtr.hpp"


struct FunctionStorage
{
	String Name{};

	VoidPtr InvokeAllParams{};
	void (*InvokeArray)(VoidPtr instance, VoidPtr ret, VoidPtr* params);
};

struct TypeStorage
{
	String Name{};
	UnorderedMap<std::size_t, SharedPtr<FunctionStorage>> FunctionsByName{};
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

	static void InvokeAllParams(VoidPtr instance, VoidPtr ret, std::remove_reference_t<Args>* ...args)
	{
		*static_cast<std::remove_reference_t<Return>*>(ret) = (static_cast<Owner*>(instance)->*MFP)(*static_cast<std::remove_reference_t<Args>*>(args)...);
	}

	static void InvokeArray(VoidPtr instance, VoidPtr ret, VoidPtr* params)
	{
		std::size_t i{sizeof...(Args)};
		*static_cast<std::remove_reference_t<Return>*>(ret) = (static_cast<Owner*>(instance)->*MFP)(*static_cast<std::remove_reference_t<Args>*>(params[--i])...);
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

private:
	TypeStorage& m_TypeStorage;
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
	Ret Invoke(VoidPtr instance, Args&&...args)
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

private:
	TypeStorage* m_TypeStorage{};
};


class AX_API Reflection
{
public:

	template<typename T>
	static auto NewType(const StringView& typeName)
	{
		auto hashName = HashValue(typeName);
		auto it = m_TypesByName.Find(hashName);
		if (it == m_TypesByName.end())
		{
			it = m_TypesByName.Insert(Pair<std::size_t, SharedPtr<TypeStorage>>(hashName, MakeShared<TypeStorage>(
				String{typeName.begin(), typeName.end()}
			))).first;
		}
		return NativeTypeHandler<T>{*it->second};
	}


	static auto FindType(const StringView& typeName)
	{
		auto hashName = HashValue(typeName);
		auto it = m_TypesByName.Find(hashName);
		if (it != m_TypesByName.end())
		{
			return TypeHandler{it->second.get()};
		}
		return TypeHandler{nullptr};
	}

private:
	static UnorderedMap<std::size_t, SharedPtr<TypeStorage>> m_TypesByName;
};