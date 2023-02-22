#pragma once

#define ENUM_FLAGS(ENUMNAME, ENUMTYPE) \
inline constexpr ENUMNAME& operator |= (ENUMNAME& a, ENUMNAME b)  noexcept { return (ENUMNAME&)(((ENUMTYPE&)a) |= ((ENUMTYPE)b)); } \
inline constexpr ENUMNAME& operator &= (ENUMNAME& a, ENUMNAME b)  noexcept { return (ENUMNAME&)(((ENUMTYPE&)a) &= ((ENUMTYPE)b)); } \
inline constexpr ENUMNAME& operator ^= (ENUMNAME& a, ENUMNAME b)  noexcept { return (ENUMNAME&)(((ENUMTYPE&)a) ^= ((ENUMTYPE)b)); } \
inline constexpr ENUMNAME& operator <<= (ENUMNAME& a, ENUMTYPE b)  noexcept { return (ENUMNAME&)(((ENUMTYPE&)a) <<= ((ENUMTYPE)b)); } \
inline constexpr ENUMNAME& operator >>= (ENUMNAME& a, ENUMTYPE b)  noexcept { return (ENUMNAME&)(((ENUMTYPE&)a) >>= ((ENUMTYPE)b)); } \
inline constexpr ENUMNAME operator | (ENUMNAME a, ENUMNAME b)	noexcept { return ENUMNAME(((ENUMTYPE)a) | ((ENUMTYPE)b));		} \
inline constexpr ENUMNAME operator & (ENUMNAME a, ENUMNAME b)	noexcept { return ENUMNAME(((ENUMTYPE)a) & ((ENUMTYPE)b));		} \
inline constexpr ENUMNAME operator ~ (ENUMNAME a)				noexcept { return ENUMNAME(~((ENUMTYPE)a));						} \
inline constexpr ENUMNAME operator ^ (ENUMNAME a, ENUMNAME b)	noexcept { return ENUMNAME(((ENUMTYPE)a) ^ (ENUMTYPE)b);		}