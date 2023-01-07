#pragma once

#include <cstdint>
#include <memory>
#include <filesystem>

#if AX_SHARED
#ifdef AX_EXPORT
		#define AX_API __declspec(dllexport)
	#else
		#define AX_API __declspec(dllimport)
	#endif
#else
	#define AX_API
#endif

#ifndef FINLINE
#	ifdef _MSC_VER
#		define FINLINE __forceinline
#   elif __CLANG__
#       define FINLINE [[clang::always_inline]] 
#	elif __GNUC__
#       define FINLINE  __attribute__((always_inline))
#   endif
#endif

#ifndef VECTORCALL
#   ifdef _MSC_VER
#		define VECTORCALL __vectorcall
#   elif __CLANG__
#       define VECTORCALL [[clang::vectorcall]] 
#	elif __GNUC__
#       define VECTORCALL  
#   endif
#endif

#if defined( __GNUC__ ) || defined(__INTEGRITY)
#define AX_ALIGNAS(_x)          __attribute__ ((aligned(_x)))
#elif defined( _WIN32) && (_MSC_VER)                                                                                   
#define AX_ALIGNAS(_x)          __declspec(align(_x))      
#else
#warning  Need to implement some method to align data here
#define  AX_ALIGNAS(_x)
#endif

#define ENUM_FLAGS(ENUMNAME, ENUMTYPE) \
inline ENUMNAME& operator |= (ENUMNAME& a, ENUMNAME b)          noexcept { return (ENUMNAME&)(((ENUMTYPE&)a) |= ((ENUMTYPE)b)); } \
inline ENUMNAME& operator &= (ENUMNAME& a, ENUMNAME b)			noexcept { return (ENUMNAME&)(((ENUMTYPE&)a) &= ((ENUMTYPE)b)); } \
inline ENUMNAME& operator ^= (ENUMNAME& a, ENUMNAME b)			noexcept { return (ENUMNAME&)(((ENUMTYPE&)a) ^= ((ENUMTYPE)b)); } \
inline constexpr ENUMNAME operator | (ENUMNAME a, ENUMNAME b)	noexcept { return ENUMNAME(((ENUMTYPE)a) | ((ENUMTYPE)b));		} \
inline constexpr ENUMNAME operator & (ENUMNAME a, ENUMNAME b)	noexcept { return ENUMNAME(((ENUMTYPE)a) & ((ENUMTYPE)b));		} \
inline constexpr ENUMNAME operator ~ (ENUMNAME a)				noexcept { return ENUMNAME(~((ENUMTYPE)a));						} \
inline constexpr ENUMNAME operator ^ (ENUMNAME a, ENUMNAME b)	noexcept { return ENUMNAME(((ENUMTYPE)a) ^ (ENUMTYPE)b);		} 

#define ax_assert(...)

using Path = std::filesystem::path;
using FileStream = std::fstream;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

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

// maybe we should move this to Algorithms.hpp
template<typename T, typename size_type = uint64> 
inline size_type Distance(const T* begin, const T* end)
{
	size_type result;
	while (begin++ < end) result++;
	return result;
}

// we can use these as std::greater, less...
namespace Compare
{
	template<typename T> inline bool Less(T a, T b) { return a < b; }
	template<typename T> inline bool Equal(T a, T b) { return a == b; }
	template<typename T> inline bool NotEqual(T a, T b) { return !Equal(a,b); }
	template<typename T> inline bool Greater(T a, T b) { return !Less(a, b) && !Equal(a, b); }
	template<typename T> inline bool GreaterEqual(T a, T b) { return !Less(a, b); }
	template<typename T> inline bool LessEqual(T a, T b) { return Less(a, b) && Equal(a, b); }
	
	/*for qsort*/ template<typename T>
	inline int QLess(const void* a, const void* b) { return *(T*)a < *(T*)b; }
	/*for qsort*/ template<typename T>
	inline int QGreater(const void* a, const void* b) { return *(T*)a > *(T*)b; }
}

enum EForceInit
{
	ForceInit
};