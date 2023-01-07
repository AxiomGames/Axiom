#pragma once

#include <memory>

template<typename T>
using SharedPtr = std::shared_ptr<T>;

template<typename T>
using WeakPtr = std::weak_ptr<T>;

template<typename T, typename Deleter = std::default_delete<T>>
using UniquePtr = std::unique_ptr<T, Deleter>;

#define DEFINE_PTR_NAMED(ClassName, TypeName) \
typedef SharedPtr<ClassName> TypeName##Ptr; \
typedef WeakPtr<ClassName> TypeName##WeakPtr;

#define FORWARD_DECL_PTR(ClassName, TypeName) \
class ClassName;                               \
typedef SharedPtr<ClassName> TypeName##Ptr; \
typedef WeakPtr<ClassName> TypeName##WeakPtr;

template<typename T, typename... Types>
static SharedPtr<T> MakeShared(Types&&... args)
{
	return std::make_shared<T>(args...);
}

template<typename T>
static SharedPtr<T> MakeShareable(T* pointer)
{
	return SharedPtr<T> { pointer };
}

template<typename T>
class SharedFromThis : public std::enable_shared_from_this<T>
{
public:
	virtual ~SharedFromThis() = default;
	template<typename B>
	std::shared_ptr<B> AsShared();

	template<typename B>
	std::shared_ptr<B> AsSharedSafe();

	inline std::shared_ptr<T> ThisShared()
	{
		return this->shared_from_this();
	}
};

template<typename T>
template<typename B>
std::shared_ptr<B> SharedFromThis<T>::AsShared()
{
	return std::static_pointer_cast<B, T>(this->shared_from_this());
}

template<typename T>
template<typename B>
std::shared_ptr<B> SharedFromThis<T>::AsSharedSafe()
{

	return std::dynamic_pointer_cast<B, T>(this->shared_from_this());
}