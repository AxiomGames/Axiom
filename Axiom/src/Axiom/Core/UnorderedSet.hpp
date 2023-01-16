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


template<typename Key, typename Alloc = DEFAULT_ALLOCATOR>
class UnorderedSet
{
public:
	UnorderedSet();

	UnorderedSet(const UnorderedSet& other);

	UnorderedSet(UnorderedSet&& other);

	~UnorderedSet();

	UnorderedSet& operator=(const UnorderedSet& other);

	UnorderedSet& operator=(UnorderedSet&& other);

	typedef AxSTL::unordered_hash_iterator<const AxSTL::unordered_hash_node<Key, void> > ConstIterator;
	typedef ConstIterator Iterator;

	Iterator begin() const;

	Iterator end() const;

	void Clear();

	bool Empty() const;

	size_t Size() const;

	Iterator Find(const Key& key) const;

	Pair<Iterator, bool> Insert(const Key& key);

	Pair<Iterator, bool> Emplace(Key&& key);

	void Erase(Iterator where);

	size_t Erase(const Key& key);

	void Swap(UnorderedSet& other);

private:

	void Rehash(size_t nbuckets);

	typedef AxSTL::unordered_hash_node<Key, void>* Pointer;

	size_t m_Size;
	AxSTL::Buffer<Pointer, Alloc> m_Buckets;
};

template<typename Key, typename Alloc>
inline UnorderedSet<Key, Alloc>::UnorderedSet()
	: m_Size(0)
{
	buffer_init<Pointer, Alloc>(&m_Buckets);
	buffer_resize<Pointer, Alloc>(&m_Buckets, 9, 0);
}

template<typename Key, typename Alloc>
inline UnorderedSet<Key, Alloc>::UnorderedSet(const UnorderedSet& other)
	: m_Size(other.m_Size)
{
	const size_t nbuckets = (size_t) (other.m_Buckets.last - other.m_Buckets.first);
	buffer_init<Pointer, Alloc>(&m_Buckets);
	buffer_resize<Pointer, Alloc>(&m_Buckets, nbuckets, 0);

	for (Pointer it = *other.m_Buckets.first; it; it = it->next)
	{
		auto newnode = new(AxSTL::PlaceHolder(), Alloc::Malloc(sizeof(AxSTL::unordered_hash_node<Key, void>))) AxSTL::unordered_hash_node<Key, void>(*it);
		newnode->next = newnode->prev = 0;
		unordered_hash_node_insert(newnode, HashValue(it->first), m_Buckets.first, nbuckets - 1);
	}
}

template<typename Key, typename Alloc>
inline UnorderedSet<Key, Alloc>::UnorderedSet(UnorderedSet&& other)
	: m_Size(other.m_Size)
{
	buffer_move(&m_Buckets, &other.m_Buckets);
	other.m_Size = 0;
}

template<typename Key, typename Alloc>
inline UnorderedSet<Key, Alloc>::~UnorderedSet()
{
	if (m_Buckets.first != m_Buckets.last)
	{
		Clear();
	}
	buffer_destroy<Pointer, Alloc>(&m_Buckets);
}

template<typename Key, typename Alloc>
inline UnorderedSet<Key, Alloc>& UnorderedSet<Key, Alloc>::operator=(const UnorderedSet<Key, Alloc>& other)
{
	UnorderedSet<Key, Alloc>(other).Swap(*this);
	return *this;
}

template<typename Key, typename Alloc>
inline UnorderedSet<Key, Alloc>& UnorderedSet<Key, Alloc>::operator=(UnorderedSet&& other)
{
	UnorderedSet(static_cast<UnorderedSet&&>(other)).Swap(*this);
	return *this;
}

template<typename Key, typename Alloc>
inline typename UnorderedSet<Key, Alloc>::Iterator UnorderedSet<Key, Alloc>::begin() const
{
	Iterator cit;
	cit.node = *m_Buckets.first;
	return cit;
}

template<typename Key, typename Alloc>
inline typename UnorderedSet<Key, Alloc>::Iterator UnorderedSet<Key, Alloc>::end() const
{
	Iterator cit;
	cit.node = 0;
	return cit;
}

template<typename Key, typename Alloc>
inline bool UnorderedSet<Key, Alloc>::Empty() const
{
	return m_Size == 0;
}

template<typename Key, typename Alloc>
inline size_t UnorderedSet<Key, Alloc>::Size() const
{
	return m_Size;
}

template<typename Key, typename Alloc>
inline void UnorderedSet<Key, Alloc>::Clear()
{
	Pointer it = *m_Buckets.first;
	while (it)
	{
		const Pointer next = it->next;
		using AxSTL::unordered_hash_node;
		it->~unordered_hash_node<Key, void>();
		Alloc::Free(it);

		it = next;
	}

	m_Buckets.last = m_Buckets.first;
	buffer_resize<Pointer, Alloc>(&m_Buckets, 9, 0);
	m_Size = 0;
}

template<typename Key, typename Alloc>
inline typename UnorderedSet<Key, Alloc>::Iterator UnorderedSet<Key, Alloc>::Find(const Key& key) const
{
	Iterator result;
	result.node = unordered_hash_find(key, m_Buckets.first, (size_t) (m_Buckets.last - m_Buckets.first));
	return result;
}

template<typename Key, typename Alloc>
inline void UnorderedSet<Key, Alloc>::Rehash(size_t nbuckets)
{
	if (m_Size + 1 > 4 * nbuckets)
	{
		Pointer root = *m_Buckets.first;

		const size_t newnbuckets = ((size_t) (m_Buckets.last - m_Buckets.first) - 1) * 8;
		m_Buckets.last = m_Buckets.first;
		buffer_resize<Pointer, Alloc>(&m_Buckets, newnbuckets + 1, 0);
		AxSTL::unordered_hash_node<Key, void>** buckets = m_Buckets.first;

		while (root)
		{
			const Pointer next = root->next;
			root->next = root->prev = 0;
			unordered_hash_node_insert(root, HashValue(root->first), buckets, newnbuckets);
			root = next;
		}
	}
}

template<typename Key, typename Alloc>
inline Pair<typename UnorderedSet<Key, Alloc>::Iterator, bool> UnorderedSet<Key, Alloc>::Insert(const Key& key)
{
	Pair<Iterator, bool> result;
	result.second = false;

	result.first = Find(key);
	if (result.first.node != 0)
		return result;

	auto newnode = new(AxSTL::PlaceHolder(), Alloc::Malloc(sizeof(AxSTL::unordered_hash_node<Key, void>))) AxSTL::unordered_hash_node<Key, void>(key);
	newnode->next = newnode->prev = 0;

	const size_t nbuckets = (size_t) (m_Buckets.last - m_Buckets.first);
	unordered_hash_node_insert(newnode, HashValue(key), m_Buckets.first, nbuckets - 1);

	++m_Size;
	Rehash(nbuckets);

	result.first.node = newnode;
	result.second = true;
	return result;
}

template<typename Key, typename Alloc>
inline Pair<typename UnorderedSet<Key, Alloc>::Iterator, bool> UnorderedSet<Key, Alloc>::Emplace(Key&& key)
{
	Pair<Iterator, bool> result;
	result.second = false;

	result.first = Find(key);
	if (result.first.node != 0)
		return result;

	const size_t keyhash = HashValue(key);
	auto newnode = new(AxSTL::PlaceHolder(), Alloc::Malloc(sizeof(AxSTL::unordered_hash_node<Key, void>))) AxSTL::unordered_hash_node<Key, void>(static_cast<Key&&>(key));
	newnode->next = newnode->prev = 0;

	const size_t nbuckets = (size_t) (m_Buckets.last - m_Buckets.first);
	unordered_hash_node_insert(newnode, keyhash, m_Buckets.first, nbuckets - 1);

	++m_Size;
	Rehash(nbuckets);

	result.first.node = newnode;
	result.second = true;
	return result;
}

template<typename Key, typename Alloc>
inline void UnorderedSet<Key, Alloc>::Erase(Iterator where)
{
	unordered_hash_node_erase(where.node, hash(where.node->first), m_Buckets.first, (size_t) (m_Buckets.last - m_Buckets.first) - 1);

	using AxSTL::unordered_hash_node;
	where.node->~unordered_hash_node<Key, void>();
	Alloc::static_deallocate((void*) where.node, sizeof(unordered_hash_node<Key, void>));
	--m_Size;
}

template<typename Key, typename Alloc>
inline size_t UnorderedSet<Key, Alloc>::Erase(const Key& key)
{
	const Iterator it = find(key);
	if (it.node == 0)
		return 0;

	erase(it);
	return 1;
}

template<typename Key, typename Alloc>
void UnorderedSet<Key, Alloc>::Swap(UnorderedSet& other)
{
	size_t tsize = other.m_Size;
	other.m_Size = m_Size, m_Size = tsize;
	buffer_swap(&m_Buckets, &other.m_Buckets);
}

