/*-
 * Copyright 2012-2018 Matthew Endsley
 * All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted providing that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "Memory.hpp"
#include "New.hpp"
#include "Traits.hpp"

#include <algorithm>

namespace AxSTL
{

	template<typename T, typename Alloc = DEFAULT_ALLOCATOR>
	struct Buffer
	{
		T* first;
		T* last;
		T* capacity;
	};

	template<typename T>
	static inline void buffer_destroy_range_traits(T* first, T* last, Traits::PodTraits<T, false>)
	{
		for (; first < last; ++first)
			first->~T();
	}

	template<typename T>
	static inline void buffer_destroy_range_traits(T*, T*, Traits::PodTraits<T, true>)
	{
	}

	template<typename T>
	static inline void buffer_destroy_range(T* first, T* last)
	{
		buffer_destroy_range_traits(first, last, Traits::PodTraits<T>());
	}

	template<typename T>
	static inline void buffer_fill_urange_traits(T* first, T* last, Traits::PodTraits<T, false>)
	{
		for (; first < last; ++first)
			new(PlaceHolder(), first) T();
	}

	template<typename T>
	static inline void buffer_fill_urange_traits(T* first, T* last, Traits::PodTraits<T, true>)
	{
		for (; first < last; ++first)
			*first = T();
	}

	template<typename T>
	static inline void buffer_fill_urange_traits(T* first, T* last, const T& value, Traits::PodTraits<T, false>)
	{
		for (; first < last; ++first)
			new(PlaceHolder(), first) T(value);
	}

	template<typename T>
	static inline void buffer_fill_urange_traits(T* first, T* last, const T& value, Traits::PodTraits<T, true>)
	{
		for (; first < last; ++first)
			*first = value;
	}

	template<typename T>
	static inline void buffer_move_urange_traits(T* dest, T* first, T* last, Traits::PodTraits<T, false>)
	{
		for (T* it = first; it != last; ++it, ++dest)
			Traits::move_construct(dest, *it);
		buffer_destroy_range(first, last);
	}

	template<typename T>
	static inline void buffer_move_urange_traits(T* dest, T* first, T* last, Traits::PodTraits<T, true>)
	{
		for (; first != last; ++first, ++dest)
			*dest = *first;
	}

	template<typename T>
	static inline void buffer_bmove_urange_traits(T* dest, T* first, T* last, Traits::PodTraits<T, false>)
	{
		dest += (last - first);
		for (T* it = last; it != first; --it, --dest)
		{
			Traits::move_construct(dest - 1, *(it - 1));
			buffer_destroy_range(it - 1, it);
		}
	}

	template<typename T>
	static inline void buffer_bmove_urange_traits(T* dest, T* first, T* last, Traits::PodTraits<T, true>)
	{
		dest += (last - first);
		for (T* it = last; it != first; --it, --dest)
			*(dest - 1) = *(it - 1);
	}

	template<typename T>
	static inline void buffer_move_urange(T* dest, T* first, T* last)
	{
		buffer_move_urange_traits(dest, first, last, Traits::PodTraits<T>());
	}

	template<typename T>
	static inline void buffer_bmove_urange(T* dest, T* first, T* last)
	{
		buffer_bmove_urange_traits(dest, first, last, Traits::PodTraits<T>());
	}

	template<typename T>
	static inline void buffer_fill_urange(T* first, T* last)
	{
		buffer_fill_urange_traits(first, last, Traits::PodTraits<T>());
	}

	template<typename T>
	static inline void buffer_fill_urange(T* first, T* last, const T& value)
	{
		buffer_fill_urange_traits(first, last, value, Traits::PodTraits<T>());
	}

	template<typename T, typename Alloc>
	static inline void buffer_init(Buffer<T, Alloc>* b)
	{
		b->first = b->last = b->capacity = 0;
	}

	template<typename T, typename Alloc>
	static inline void buffer_destroy(Buffer<T, Alloc>* b)
	{
		buffer_destroy_range(b->first, b->last);
		Alloc::Free(b->first);
	}

	template<typename T, typename Alloc>
	static inline void buffer_reserve(Buffer<T, Alloc>* b, size_t capacity)
	{
		if (b->first + capacity <= b->capacity)
		{
			return;
		}

		typedef T* pointer;
		const size_t size = (size_t) (b->last - b->first);
		pointer newfirst = (pointer) Alloc::Malloc(sizeof(T) * capacity);
		buffer_move_urange(newfirst, b->first, b->last);
		Alloc::Free(b->first);

		b->first = newfirst;
		b->last = newfirst + size;
		b->capacity = newfirst + capacity;
	}

	template<typename T, typename Alloc>
	static inline void buffer_resize(Buffer<T, Alloc>* b, size_t size)
	{
		buffer_reserve(b, size);

		buffer_fill_urange(b->last, b->first + size);
		buffer_destroy_range(b->first + size, b->last);
		b->last = b->first + size;
	}

	template<typename T, typename Alloc>
	static inline void buffer_resize(Buffer<T, Alloc>* b, size_t size, const T& value)
	{
		buffer_reserve(b, size);

		buffer_fill_urange(b->last, b->first + size, value);
		buffer_destroy_range(b->first + size, b->last);
		b->last = b->first + size;
	}

	template<typename T, typename Alloc>
	static inline void buffer_shrink_to_fit(Buffer<T, Alloc>* b)
	{
		if (b->capacity != b->last)
		{
			if (b->last == b->first)
			{
				const size_t capacity = (size_t) (b->capacity - b->first);
				Alloc::Free(b->first, sizeof(T) * capacity);
				b->capacity = b->first = b->last = nullptr;
			}
			else
			{
				const size_t capacity = (size_t) (b->capacity - b->first);
				const size_t size = (size_t) (b->last - b->first);
				T* newfirst = (T*) Alloc::Malloc(sizeof(T) * size);
				buffer_move_urange(newfirst, b->first, b->last);
				Alloc::Free(b->first, sizeof(T) * capacity);
				b->first = newfirst;
				b->last = newfirst + size;
				b->capacity = b->last;
			}
		}
	}

	template<typename T, typename Alloc>
	static inline void buffer_clear(Buffer<T, Alloc>* b)
	{
		buffer_destroy_range(b->first, b->last);
		b->last = b->first;
	}

	template<typename T, typename Alloc>
	static inline T* buffer_insert_common(Buffer<T, Alloc>* b, T* where, size_t count)
	{
		const size_t offset = (size_t) (where - b->first);
		const size_t newsize = (size_t) ((b->last - b->first) + count);
		if (b->first + newsize > b->capacity)
		{
			buffer_reserve(b, (newsize * 3) / 2);
		}

		where = b->first + offset;

		if (where != b->last)
		{
			buffer_bmove_urange(where + count, where, b->last);
		}

		b->last = b->first + newsize;

		return where;
	}

	template<typename T, typename Alloc, typename Param>
	static inline void buffer_insert(Buffer<T, Alloc>* b, T* where, const Param* first, const Param* last)
	{
		typedef const char* pointer;
		const size_t count = last - first;
		const bool frombuf = ((pointer) b->first <= (pointer) first && (pointer) b->last >= (pointer) last);
		size_t offset;
		if (frombuf)
		{
			offset = (pointer) first - (pointer) b->first;
			if ((pointer) where <= (pointer) first)
			{
				offset += count * sizeof(T);
			}
			where = buffer_insert_common(b, where, count);
			first = (Param*) ((pointer) b->first + offset);
			last = first + count;
		}
		else
		{
			where = buffer_insert_common(b, where, count);
		}
		for (; first != last; ++first, ++where)
			new(PlaceHolder(), where) T(*first);
	}

	template<typename T, typename Alloc>
	static inline void buffer_insert(Buffer<T, Alloc>* b, T* where, size_t count)
	{
		where = buffer_insert_common(b, where, count);
		for (T* end = where + count; where != end; ++where)
			new(PlaceHolder(), where) T();
	}

	template<typename T, typename Alloc, typename Param>
	static inline void buffer_append(Buffer<T, Alloc>* b, const Param* param)
	{
		if (b->capacity != b->last)
		{
			new(PlaceHolder(), b->last) T(*param);
			++b->last;
		}
		else
		{
			buffer_insert(b, b->last, param, param + 1);
		}
	}

	template<typename T, typename Alloc>
	static inline void buffer_append(Buffer<T, Alloc>* b)
	{
		if (b->capacity != b->last)
		{
			new(PlaceHolder(), b->last) T();
			++b->last;
		}
		else
		{
			buffer_insert(b, b->last, 1);
		}
	}

	template<typename T, typename Alloc, typename ...Args>
	static inline T* buffer_emplace(Buffer<T, Alloc>* b, Args&& ... args)
	{
		auto where = b->last;
		if (where == b->capacity)
		{
			where = buffer_insert_common(b, b->last, 1);
		}
		else
		{
			++b->last;
		}
		if constexpr (std::is_aggregate_v<T>)
		{
			new(PlaceHolder(), where) T{std::forward<Args>(args)...};
		}
		else
		{
			new(PlaceHolder(), where) T(std::forward<Args>(args)...);
		}
		return where;
	}

	template<typename T, typename Alloc>
	static inline T* buffer_erase(Buffer<T, Alloc>* b, T* first, T* last)
	{
		typedef T* pointer;
		const size_t count = (last - first);
		for (pointer it = last, end = b->last, dest = first; it != end; ++it, ++dest)
			Traits::move(*dest, *it);

		buffer_destroy_range(b->last - count, b->last);

		b->last -= count;
		return first;
	}

	template<typename T, typename Alloc>
	static inline T* buffer_erase_unordered(Buffer<T, Alloc>* b, T* first, T* last)
	{
		typedef T* pointer;
		const size_t count = (last - first);
		const size_t tail = (b->last - last);
		pointer it = b->last - ((count < tail) ? count : tail);
		for (pointer end = b->last, dest = first; it != end; ++it, ++dest)
			Traits::move(*dest, *it);

		buffer_destroy_range(b->last - count, b->last);

		b->last -= count;
		return first;
	}

	template<typename T, typename Alloc>
	static inline void buffer_swap(Buffer<T, Alloc>* b, Buffer<T, Alloc>* other)
	{
		typedef T* pointer;
		const pointer tfirst = b->first, tlast = b->last, tcapacity = b->capacity;
		b->first = other->first, b->last = other->last, b->capacity = other->capacity;
		other->first = tfirst, other->last = tlast, other->capacity = tcapacity;
	}

	template<typename T, typename Alloc>
	static inline void buffer_move(Buffer<T, Alloc>* dst, Buffer<T, Alloc>* src)
	{
		dst->first = src->first, dst->last = src->last, dst->capacity = src->capacity;
		src->first = src->last = src->capacity = nullptr;
	}
}
