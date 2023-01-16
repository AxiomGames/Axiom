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

	typedef AxSTL::unordered_hash_iterator<const AxSTL::unordered_hash_node<Key, void> > const_iterator;
	typedef const_iterator iterator;

	iterator begin() const;

	iterator end() const;

	void clear();

	bool empty() const;

	size_t size() const;

	iterator find(const Key& key) const;

	Pair<iterator, bool> insert(const Key& key);

	Pair<iterator, bool> emplace(Key&& key);

	void erase(iterator where);

	size_t erase(const Key& key);

	void swap(UnorderedSet& other);

private:

	void rehash(size_t nbuckets);

	typedef AxSTL::unordered_hash_node<Key, void>* pointer;

	size_t m_size;
	AxSTL::Buffer<pointer, Alloc> m_buckets;
};

template<typename Key, typename Alloc>
inline UnorderedSet<Key, Alloc>::UnorderedSet()
	: m_size(0)
{
	buffer_init<pointer, Alloc>(&m_buckets);
	buffer_resize<pointer, Alloc>(&m_buckets, 9, 0);
}

template<typename Key, typename Alloc>
inline UnorderedSet<Key, Alloc>::UnorderedSet(const UnorderedSet& other)
	: m_size(other.m_size)
{
	const size_t nbuckets = (size_t) (other.m_buckets.last - other.m_buckets.first);
	buffer_init<pointer, Alloc>(&m_buckets);
	buffer_resize<pointer, Alloc>(&m_buckets, nbuckets, 0);

	for (pointer it = *other.m_buckets.first; it; it = it->next)
	{
		auto newnode = new(AxSTL::PlaceHolder(), Alloc::Malloc(sizeof(AxSTL::unordered_hash_node<Key, void>))) AxSTL::unordered_hash_node<Key, void>(*it);
		newnode->next = newnode->prev = 0;
		unordered_hash_node_insert(newnode, HashValue(it->first), m_buckets.first, nbuckets - 1);
	}
}

template<typename Key, typename Alloc>
inline UnorderedSet<Key, Alloc>::UnorderedSet(UnorderedSet&& other)
	: m_size(other.m_size)
{
	buffer_move(&m_buckets, &other.m_buckets);
	other.m_size = 0;
}

template<typename Key, typename Alloc>
inline UnorderedSet<Key, Alloc>::~UnorderedSet()
{
	if (m_buckets.first != m_buckets.last)
		clear();
	buffer_destroy<pointer, Alloc>(&m_buckets);
}

template<typename Key, typename Alloc>
inline UnorderedSet<Key, Alloc>& UnorderedSet<Key, Alloc>::operator=(const UnorderedSet<Key, Alloc>& other)
{
	UnorderedSet<Key, Alloc>(other).swap(*this);
	return *this;
}

template<typename Key, typename Alloc>
inline UnorderedSet<Key, Alloc>& UnorderedSet<Key, Alloc>::operator=(UnorderedSet&& other)
{
	UnorderedSet(static_cast<UnorderedSet&&>(other)).swap(*this);
	return *this;
}

template<typename Key, typename Alloc>
inline typename UnorderedSet<Key, Alloc>::iterator UnorderedSet<Key, Alloc>::begin() const
{
	iterator cit;
	cit.node = *m_buckets.first;
	return cit;
}

template<typename Key, typename Alloc>
inline typename UnorderedSet<Key, Alloc>::iterator UnorderedSet<Key, Alloc>::end() const
{
	iterator cit;
	cit.node = 0;
	return cit;
}

template<typename Key, typename Alloc>
inline bool UnorderedSet<Key, Alloc>::empty() const
{
	return m_size == 0;
}

template<typename Key, typename Alloc>
inline size_t UnorderedSet<Key, Alloc>::size() const
{
	return m_size;
}

template<typename Key, typename Alloc>
inline void UnorderedSet<Key, Alloc>::clear()
{
	pointer it = *m_buckets.first;
	while (it)
	{
		const pointer next = it->next;
		using AxSTL::unordered_hash_node;
		it->~unordered_hash_node<Key, void>();
		Alloc::Free(it);

		it = next;
	}

	m_buckets.last = m_buckets.first;
	buffer_resize<pointer, Alloc>(&m_buckets, 9, 0);
	m_size = 0;
}

template<typename Key, typename Alloc>
inline typename UnorderedSet<Key, Alloc>::iterator UnorderedSet<Key, Alloc>::find(const Key& key) const
{
	iterator result;
	result.node = unordered_hash_find(key, m_buckets.first, (size_t) (m_buckets.last - m_buckets.first));
	return result;
}

template<typename Key, typename Alloc>
inline void UnorderedSet<Key, Alloc>::rehash(size_t nbuckets)
{
	if (m_size + 1 > 4 * nbuckets)
	{
		pointer root = *m_buckets.first;

		const size_t newnbuckets = ((size_t) (m_buckets.last - m_buckets.first) - 1) * 8;
		m_buckets.last = m_buckets.first;
		buffer_resize<pointer, Alloc>(&m_buckets, newnbuckets + 1, 0);
		AxSTL::unordered_hash_node<Key, void>** buckets = m_buckets.first;

		while (root)
		{
			const pointer next = root->next;
			root->next = root->prev = 0;
			unordered_hash_node_insert(root, HashValue(root->first), buckets, newnbuckets);
			root = next;
		}
	}
}

template<typename Key, typename Alloc>
inline Pair<typename UnorderedSet<Key, Alloc>::iterator, bool> UnorderedSet<Key, Alloc>::insert(const Key& key)
{
	Pair<iterator, bool> result;
	result.second = false;

	result.first = find(key);
	if (result.first.node != 0)
		return result;

	auto newnode = new(AxSTL::PlaceHolder(), Alloc::Malloc(sizeof(AxSTL::unordered_hash_node<Key, void>))) AxSTL::unordered_hash_node<Key, void>(key);
	newnode->next = newnode->prev = 0;

	const size_t nbuckets = (size_t) (m_buckets.last - m_buckets.first);
	unordered_hash_node_insert(newnode, HashValue(key), m_buckets.first, nbuckets - 1);

	++m_size;
	rehash(nbuckets);

	result.first.node = newnode;
	result.second = true;
	return result;
}

template<typename Key, typename Alloc>
inline Pair<typename UnorderedSet<Key, Alloc>::iterator, bool> UnorderedSet<Key, Alloc>::emplace(Key&& key)
{
	Pair<iterator, bool> result;
	result.second = false;

	result.first = find(key);
	if (result.first.node != 0)
		return result;

	const size_t keyhash = HashValue(key);
	auto newnode = new(AxSTL::PlaceHolder(), Alloc::Malloc(sizeof(AxSTL::unordered_hash_node<Key, void>))) AxSTL::unordered_hash_node<Key, void>(static_cast<Key&&>(key));
	newnode->next = newnode->prev = 0;

	const size_t nbuckets = (size_t) (m_buckets.last - m_buckets.first);
	unordered_hash_node_insert(newnode, keyhash, m_buckets.first, nbuckets - 1);

	++m_size;
	rehash(nbuckets);

	result.first.node = newnode;
	result.second = true;
	return result;
}

template<typename Key, typename Alloc>
inline void UnorderedSet<Key, Alloc>::erase(iterator where)
{
	unordered_hash_node_erase(where.node, hash(where.node->first), m_buckets.first, (size_t) (m_buckets.last - m_buckets.first) - 1);

	using AxSTL::unordered_hash_node;
	where.node->~unordered_hash_node<Key, void>();
	Alloc::static_deallocate((void*) where.node, sizeof(unordered_hash_node<Key, void>));
	--m_size;
}

template<typename Key, typename Alloc>
inline size_t UnorderedSet<Key, Alloc>::erase(const Key& key)
{
	const iterator it = find(key);
	if (it.node == 0)
		return 0;

	erase(it);
	return 1;
}

template<typename Key, typename Alloc>
void UnorderedSet<Key, Alloc>::swap(UnorderedSet& other)
{
	size_t tsize = other.m_size;
	other.m_size = m_size, m_size = tsize;
	buffer_swap(&m_buckets, &other.m_buckets);
}

