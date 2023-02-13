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
#include "Buffer.hpp"
#include "Hash.hpp"
#include "HashBase.hpp"


template<typename Key, typename Value, typename Alloc = DEFAULT_ALLOCATOR>
class UnorderedMap
{
public:
	UnorderedMap();

	UnorderedMap(const UnorderedMap& other);

	UnorderedMap(UnorderedMap&& other);

	~UnorderedMap();

	UnorderedMap& operator=(const UnorderedMap& other);

	UnorderedMap& operator=(UnorderedMap&& other);

	typedef Pair<Key, Value> ValueType;

	typedef AxSTL::UnorderedHashIterator<const AxSTL::UnorderedHashNode<Key, Value> > ConstIterator;
	typedef AxSTL::UnorderedHashIterator<AxSTL::UnorderedHashNode<Key, Value>> Iterator;

	Iterator begin();

	Iterator end();

	ConstIterator begin() const;

	ConstIterator end() const;

	void Clear();

	bool Empty() const;

	size_t Size() const;

	ConstIterator Find(const Key& key) const;

	Iterator Find(const Key& key);

	Pair<Iterator, bool> Insert(const Pair<Key, Value>& p);

	Pair<Iterator, bool> Emplace(Pair<Key, Value>&& p);

	void Erase(ConstIterator where);

	Value& operator[](const Key& key);

	void Swap(UnorderedMap& other);

private:

	void Rehash(size_t nbuckets);

	typedef AxSTL::UnorderedHashNode<Key, Value>* pointer;

	size_t m_size;
	AxSTL::Buffer<pointer, Alloc> m_buckets;
};

template<typename Key, typename Value, typename Alloc>
inline UnorderedMap<Key, Value, Alloc>::UnorderedMap()
	: m_size(0)
{
	buffer_init<pointer, Alloc>(&m_buckets);
}

template<typename Key, typename Value, typename Alloc>
inline UnorderedMap<Key, Value, Alloc>::UnorderedMap(const UnorderedMap& other)
	: m_size(other.m_size)
{
	buffer_init<pointer, Alloc>(&m_buckets);
	if (!other.m_buckets.first) return;

	const size_t nbuckets = (size_t) (other.m_buckets.last - other.m_buckets.first);
	buffer_resize<pointer, Alloc>(&m_buckets, nbuckets, 0);

	for (pointer it = *other.m_buckets.first; it; it = it->next)
	{
		auto newnode = new(AxSTL::PlaceHolder(), Alloc::Malloc(sizeof(AxSTL::UnorderedHashNode<Key, Value>))) AxSTL::UnorderedHashNode<Key, Value>(
			it->first, it->second);
		newnode->next = newnode->prev = 0;

		AxSTL::unordered_hash_node_insert(newnode,  Hash<Key>::hash(it->first), m_buckets.first, nbuckets - 1);
	}
}

template<typename Key, typename Value, typename Alloc>
inline UnorderedMap<Key, Value, Alloc>::UnorderedMap(UnorderedMap&& other)
	: m_size(other.m_size)
{
	buffer_move(&m_buckets, &other.m_buckets);
	other.m_size = 0;
}

template<typename Key, typename Value, typename Alloc>
inline UnorderedMap<Key, Value, Alloc>::~UnorderedMap()
{
	if (m_buckets.first != m_buckets.last)
	{
		Clear();
	}
	buffer_destroy<pointer, Alloc>(&m_buckets);
}

template<typename Key, typename Value, typename Alloc>
inline UnorderedMap<Key, Value, Alloc>& UnorderedMap<Key, Value, Alloc>::operator=(const UnorderedMap<Key, Value, Alloc>& other)
{
	UnorderedMap<Key, Value, Alloc>(other).Swap(*this);
	return *this;
}

template<typename Key, typename Value, typename Alloc>
inline UnorderedMap<Key, Value, Alloc>& UnorderedMap<Key, Value, Alloc>::operator=(UnorderedMap&& other)
{
	UnorderedMap(static_cast<UnorderedMap&&>(other)).Swap(*this);
	return *this;
}

template<typename Key, typename Value, typename Alloc>
inline typename UnorderedMap<Key, Value, Alloc>::Iterator UnorderedMap<Key, Value, Alloc>::begin()
{
	Iterator it;
	it.node = m_buckets.first ? *m_buckets.first : 0;
	return it;
}

template<typename Key, typename Value, typename Alloc>
inline typename UnorderedMap<Key, Value, Alloc>::Iterator UnorderedMap<Key, Value, Alloc>::end()
{
	Iterator it;
	it.node = 0;
	return it;
}

template<typename Key, typename Value, typename Alloc>
inline typename UnorderedMap<Key, Value, Alloc>::ConstIterator UnorderedMap<Key, Value, Alloc>::begin() const
{
	ConstIterator cit;
	cit.node = m_buckets.first ? *m_buckets.first : 0;
	return cit;
}

template<typename Key, typename Value, typename Alloc>
inline typename UnorderedMap<Key, Value, Alloc>::ConstIterator UnorderedMap<Key, Value, Alloc>::end() const
{
	ConstIterator cit;
	cit.node = 0;
	return cit;
}

template<typename Key, typename Value, typename Alloc>
inline bool UnorderedMap<Key, Value, Alloc>::Empty() const
{
	return m_size == 0;
}

template<typename Key, typename Value, typename Alloc>
inline size_t UnorderedMap<Key, Value, Alloc>::Size() const
{
	return m_size;
}

template<typename Key, typename Value, typename Alloc>
inline void UnorderedMap<Key, Value, Alloc>::Clear()
{
	if (!m_buckets.first) return;

	pointer it = *m_buckets.first;
	while (it)
	{
		const pointer next = it->next;

		using AxSTL::UnorderedHashNode;
		it->~UnorderedHashNode<Key, Value>();
		Alloc::Free(it);

		it = next;
	}

	m_buckets.last = m_buckets.first;
	buffer_resize<pointer, Alloc>(&m_buckets, 9, 0);
	m_size = 0;
}

template<typename Key, typename Value, typename Alloc>
inline typename UnorderedMap<Key, Value, Alloc>::Iterator UnorderedMap<Key, Value, Alloc>::Find(const Key& key)
{
	Iterator result;
	result.node = unordered_hash_find(key, m_buckets.first, (size_t) (m_buckets.last - m_buckets.first));
	return result;
}

template<typename Key, typename Value, typename Alloc>
inline typename UnorderedMap<Key, Value, Alloc>::ConstIterator UnorderedMap<Key, Value, Alloc>::Find(const Key& key) const
{
	Iterator result;
	result.node = unordered_hash_find(key, m_buckets.first, (size_t) (m_buckets.last - m_buckets.first));
	return result;
}

template<typename Key, typename Value, typename Alloc>
inline void UnorderedMap<Key, Value, Alloc>::Rehash(size_t nbuckets)
{
	if (m_size + 1 > 4 * nbuckets)
	{
		pointer root = *m_buckets.first;

		const size_t newnbuckets = ((size_t) (m_buckets.last - m_buckets.first) - 1) * 8;
		m_buckets.last = m_buckets.first;
		AxSTL::buffer_resize<pointer, Alloc>(&m_buckets, newnbuckets + 1, 0);
		AxSTL::UnorderedHashNode<Key, Value>** buckets = m_buckets.first;

		while (root)
		{
			const pointer next = root->next;
			root->next = root->prev = 0;
			unordered_hash_node_insert(root, HashValue(root->first), buckets, newnbuckets);
			root = next;
		}
	}
}

template<typename Key, typename Value, typename Alloc>
inline Pair<typename UnorderedMap<Key, Value, Alloc>::Iterator, bool> UnorderedMap<Key, Value, Alloc>::Insert(const Pair<Key, Value>& p)
{
	Pair<Iterator, bool> result;
	result.second = false;

	result.first = Find(p.first);
	if (result.first.node != 0)
		return result;

	auto newnode = new(AxSTL::PlaceHolder(), Alloc::Malloc(sizeof(AxSTL::UnorderedHashNode<Key, Value>))) AxSTL::UnorderedHashNode<Key, Value>(p.first, p.second);
	newnode->next = newnode->prev = 0;

	if (!m_buckets.first) buffer_resize<pointer, Alloc>(&m_buckets, 9, 0);
	const size_t nbuckets = (size_t) (m_buckets.last - m_buckets.first);
	unordered_hash_node_insert(newnode, HashValue(p.first), m_buckets.first, nbuckets - 1);

	++m_size;
	Rehash(nbuckets);

	result.first.node = newnode;
	result.second = true;
	return result;
}

template<typename Key, typename Value, typename Alloc>
inline Pair<typename UnorderedMap<Key, Value, Alloc>::Iterator, bool> UnorderedMap<Key, Value, Alloc>::Emplace(Pair<Key, Value>&& p)
{
	Pair<Iterator, bool> result;
	result.second = false;

	result.first = Find(p.first);
	if (result.first.node != 0)
		return result;

	const size_t keyhash = HashValue(p.first);
	auto newnode = new(AxSTL::PlaceHolder(), Alloc::Malloc(sizeof(AxSTL::UnorderedHashNode<Key, Value>))) AxSTL::UnorderedHashNode<Key, Value>(static_cast<Key&&>(p.first),
		static_cast<Value&&>(p.second));
	newnode->next = newnode->prev = 0;

	if (!m_buckets.first) buffer_resize<pointer, Alloc>(&m_buckets, 9, 0);
	const size_t nbuckets = (size_t) (m_buckets.last - m_buckets.first);
	unordered_hash_node_insert(newnode, keyhash, m_buckets.first, nbuckets - 1);

	++m_size;
	Rehash(nbuckets);

	result.first.node = newnode;
	result.second = true;
	return result;
}

template<typename Key, typename Value, typename Alloc>
inline void UnorderedMap<Key, Value, Alloc>::Erase(ConstIterator where)
{
	unordered_hash_node_erase(where.node, hash(where->first), m_buckets.first, (size_t) (m_buckets.last - m_buckets.first) - 1);

	using AxSTL::UnorderedHashNode;
	where->~UnorderedHashNode<Key, Value>();
	Alloc::Free((void*) where.node);
	--m_size;
}

template<typename Key, typename Value, typename Alloc>
inline Value& UnorderedMap<Key, Value, Alloc>::operator[](const Key& key)
{
	return Insert(Pair<Key, Value>(key, Value())).first->second;
}

template<typename Key, typename Value, typename Alloc>
inline void UnorderedMap<Key, Value, Alloc>::Swap(UnorderedMap& other)
{
	size_t tsize = other.m_size;
	other.m_size = m_size, m_size = tsize;
	buffer_swap(&m_buckets, &other.m_buckets);
}