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

	typedef AxSTL::Pair<Key, Value> value_type;

	typedef AxSTL::unordered_hash_iterator<const AxSTL::unordered_hash_node<Key, Value> > const_iterator;
	typedef AxSTL::unordered_hash_iterator<AxSTL::unordered_hash_node<Key, Value>> iterator;

	iterator begin();

	iterator end();

	const_iterator begin() const;

	const_iterator end() const;

	void clear();

	bool empty() const;

	size_t size() const;

	const_iterator find(const Key& key) const;

	iterator find(const Key& key);

	AxSTL::Pair<iterator, bool> insert(const AxSTL::Pair<Key, Value>& p);

	AxSTL::Pair<iterator, bool> emplace(AxSTL::Pair<Key, Value>&& p);

	void erase(const_iterator where);

	Value& operator[](const Key& key);

	void swap(UnorderedMap& other);

private:

	void rehash(size_t nbuckets);

	typedef AxSTL::unordered_hash_node<Key, Value>* pointer;

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
	const size_t nbuckets = (size_t) (other.m_buckets.last - other.m_buckets.first);
	buffer_init<pointer, Alloc>(&m_buckets);
	buffer_resize<pointer, Alloc>(&m_buckets, nbuckets, 0);

	for (pointer it = *other.m_buckets.first; it; it = it->next)
	{
		auto newnode = new(AxSTL::PlaceHolder(), Alloc::static_allocate(sizeof(AxSTL::unordered_hash_node<Key, Value>))) AxSTL::unordered_hash_node<Key, Value>(
			it->first, it->second);
		newnode->next = newnode->prev = 0;

		AxSTL::unordered_hash_node_insert(newnode, hash(it->first), m_buckets.first, nbuckets - 1);
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
		clear();
	buffer_destroy<pointer, Alloc>(&m_buckets);
}

template<typename Key, typename Value, typename Alloc>
inline UnorderedMap<Key, Value, Alloc>& UnorderedMap<Key, Value, Alloc>::operator=(const UnorderedMap<Key, Value, Alloc>& other)
{
	UnorderedMap<Key, Value, Alloc>(other).swap(*this);
	return *this;
}

template<typename Key, typename Value, typename Alloc>
inline UnorderedMap<Key, Value, Alloc>& UnorderedMap<Key, Value, Alloc>::operator=(UnorderedMap&& other)
{
	UnorderedMap(static_cast<UnorderedMap&&>(other)).swap(*this);
	return *this;
}

template<typename Key, typename Value, typename Alloc>
inline typename UnorderedMap<Key, Value, Alloc>::iterator UnorderedMap<Key, Value, Alloc>::begin()
{
	iterator it;
	it.node = *m_buckets.first;
	return it;
}

template<typename Key, typename Value, typename Alloc>
inline typename UnorderedMap<Key, Value, Alloc>::iterator UnorderedMap<Key, Value, Alloc>::end()
{
	iterator it;
	it.node = 0;
	return it;
}

template<typename Key, typename Value, typename Alloc>
inline typename UnorderedMap<Key, Value, Alloc>::const_iterator UnorderedMap<Key, Value, Alloc>::begin() const
{
	const_iterator cit;
	cit.node = *m_buckets.first;
	return cit;
}

template<typename Key, typename Value, typename Alloc>
inline typename UnorderedMap<Key, Value, Alloc>::const_iterator UnorderedMap<Key, Value, Alloc>::end() const
{
	const_iterator cit;
	cit.node = 0;
	return cit;
}

template<typename Key, typename Value, typename Alloc>
inline bool UnorderedMap<Key, Value, Alloc>::empty() const
{
	return m_size == 0;
}

template<typename Key, typename Value, typename Alloc>
inline size_t UnorderedMap<Key, Value, Alloc>::size() const
{
	return m_size;
}

template<typename Key, typename Value, typename Alloc>
inline void UnorderedMap<Key, Value, Alloc>::clear()
{
	pointer it = *m_buckets.first;
	while (it)
	{
		const pointer next = it->next;
		it->~unordered_hash_node<Key, Value>();
		Alloc::static_deallocate(it, sizeof(AxSTL::unordered_hash_node < Key, Value >));

		it = next;
	}

	m_buckets.last = m_buckets.first;
	buffer_resize<pointer, Alloc>(&m_buckets, 9, 0);
	m_size = 0;
}

template<typename Key, typename Value, typename Alloc>
inline typename UnorderedMap<Key, Value, Alloc>::iterator UnorderedMap<Key, Value, Alloc>::find(const Key& key)
{
	iterator result;
	result.node = unordered_hash_find(key, m_buckets.first, (size_t) (m_buckets.last - m_buckets.first));
	return result;
}

template<typename Key, typename Value, typename Alloc>
inline typename UnorderedMap<Key, Value, Alloc>::const_iterator UnorderedMap<Key, Value, Alloc>::find(const Key& key) const
{
	iterator result;
	result.node = unordered_hash_find(key, m_buckets.first, (size_t) (m_buckets.last - m_buckets.first));
	return result;
}

template<typename Key, typename Value, typename Alloc>
inline void UnorderedMap<Key, Value, Alloc>::rehash(size_t nbuckets)
{
	if (m_size + 1 > 4 * nbuckets)
	{
		pointer root = *m_buckets.first;

		const size_t newnbuckets = ((size_t) (m_buckets.last - m_buckets.first) - 1) * 8;
		m_buckets.last = m_buckets.first;
		buffer_resize<pointer, Alloc>(&m_buckets, newnbuckets + 1, 0);
		AxSTL::unordered_hash_node <Key, Value>** buckets = m_buckets.first;

		while (root)
		{
			const pointer next = root->next;
			root->next = root->prev = 0;
			unordered_hash_node_insert(root, hash(root->first), buckets, newnbuckets);
			root = next;
		}
	}
}

template<typename Key, typename Value, typename Alloc>
inline AxSTL::Pair<typename UnorderedMap<Key, Value, Alloc>::iterator, bool> UnorderedMap<Key, Value, Alloc>::insert(const AxSTL::Pair <Key, Value>& p)
{
	AxSTL::Pair<iterator, bool> result;
	result.second = false;

	result.first = find(p.first);
	if (result.first.node != 0)
		return result;

	auto newnode = new(AxSTL::PlaceHolder(), Alloc::static_allocate(sizeof(AxSTL::unordered_hash_node < Key, Value >))) AxSTL::unordered_hash_node<Key, Value>(p.first, p.second);
	newnode->next = newnode->prev = 0;

	if (!m_buckets.first) buffer_resize<pointer, Alloc>(&m_buckets, 9, 0);
	const size_t nbuckets = (size_t) (m_buckets.last - m_buckets.first);
	unordered_hash_node_insert(newnode, hash(p.first), m_buckets.first, nbuckets - 1);

	++m_size;
	rehash(nbuckets);

	result.first.node = newnode;
	result.second = true;
	return result;
}

template<typename Key, typename Value, typename Alloc>
inline Pair<typename UnorderedMap<Key, Value, Alloc>::iterator, bool> UnorderedMap<Key, Value, Alloc>::emplace(Pair <Key, Value>&& p)
{
	Pair<iterator, bool> result;
	result.second = false;

	result.first = find(p.first);
	if (result.first.node != 0)
		return result;

	const size_t keyhash = hash(p.first);
	unordered_hash_node <Key, Value>* newnode = new(placeholder(), Alloc::static_allocate(sizeof(unordered_hash_node < Key, Value >))) unordered_hash_node<Key, Value>(static_cast<Key&&>(p.first),
		static_cast<Value&&>(p.second));
	newnode->next = newnode->prev = 0;

	if (!m_buckets.first) buffer_resize<pointer, Alloc>(&m_buckets, 9, 0);
	const size_t nbuckets = (size_t) (m_buckets.last - m_buckets.first);
	unordered_hash_node_insert(newnode, keyhash, m_buckets.first, nbuckets - 1);

	++m_size;
	rehash(nbuckets);

	result.first.node = newnode;
	result.second = true;
	return result;
}

template<typename Key, typename Value, typename Alloc>
inline void UnorderedMap<Key, Value, Alloc>::erase(const_iterator where)
{
	unordered_hash_node_erase(where.node, hash(where->first), m_buckets.first, (size_t) (m_buckets.last - m_buckets.first) - 1);

	where->~unordered_hash_node<Key, Value>();
	Alloc::static_deallocate((void*) where.node, sizeof(AxSTL::unordered_hash_node < Key, Value >));
	--m_size;
}

template<typename Key, typename Value, typename Alloc>
inline Value& UnorderedMap<Key, Value, Alloc>::operator[](const Key& key)
{
	return insert(Pair<Key, Value>(key, Value())).first->second;
}

template<typename Key, typename Value, typename Alloc>
inline void UnorderedMap<Key, Value, Alloc>::swap(UnorderedMap& other)
{
	size_t tsize = other.m_size;
	other.m_size = m_size, m_size = tsize;
	buffer_swap(&m_buckets, &other.m_buckets);
}