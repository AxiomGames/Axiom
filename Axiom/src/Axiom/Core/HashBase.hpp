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

#ifndef TINYSTL_HASH_BASE_H
#define TINYSTL_HASH_BASE_H

#include "Common.hpp"
#include "Traits.hpp"
#include "Pair.hpp"

namespace AxSTL
{

	template<typename Key, typename Value>
	struct UnorderedHashNode
	{
		UnorderedHashNode(const Key& key, const Value& value);

		UnorderedHashNode(Key&& key, Value&& value);

		const Key first;
		Value second;
		UnorderedHashNode* next;
		UnorderedHashNode* prev;

	private:
		UnorderedHashNode& operator=(const UnorderedHashNode&);
	};

	template<typename Key, typename Value>
	inline UnorderedHashNode<Key, Value>::UnorderedHashNode(const Key& key, const Value& value)
		: first(key), second(value)
	{
	}

	template<typename Key, typename Value>
	inline UnorderedHashNode<Key, Value>::UnorderedHashNode(Key&& key, Value&& value)
		: first(static_cast<Key&&>(key)), second(static_cast<Value&&>(value))
	{
	}

	template<typename Key>
	struct UnorderedHashNode<Key, void>
	{
		explicit UnorderedHashNode(const Key& key);

		explicit UnorderedHashNode(Key&& key);

		const Key first;
		UnorderedHashNode* next;
		UnorderedHashNode* prev;

	private:
		UnorderedHashNode& operator=(const UnorderedHashNode&);
	};

	template<typename Key>
	inline UnorderedHashNode<Key, void>::UnorderedHashNode(const Key& key)
		: first(key)
	{
	}

	template<typename Key>
	inline UnorderedHashNode<Key, void>::UnorderedHashNode(Key&& key)
		: first(static_cast<Key&&>(key))
	{
	}

	template<typename Key, typename Value>
	static inline void unordered_hash_node_insert(UnorderedHashNode<Key, Value>* node, size_t hash, UnorderedHashNode<Key, Value>** buckets, size_t nbuckets)
	{
		size_t bucket = hash & (nbuckets - 1);

		UnorderedHashNode<Key, Value>* it = buckets[bucket + 1];
		node->next = it;
		if (it)
		{
			node->prev = it->prev;
			it->prev = node;
			if (node->prev)
				node->prev->next = node;
		}
		else
		{
			size_t newbucket = bucket;
			while (newbucket && !buckets[newbucket])
				--newbucket;

			UnorderedHashNode<Key, Value>* prev = buckets[newbucket];
			while (prev && prev->next)
				prev = prev->next;

			node->prev = prev;
			if (prev)
				prev->next = node;
		}

		// propagate node through buckets
		for (; it == buckets[bucket]; --bucket)
		{
			buckets[bucket] = node;
			if (!bucket)
				break;
		}
	}

	template<typename Key, typename Value>
	static inline void unordered_hash_node_erase(const UnorderedHashNode<Key, Value>* where, size_t hash, UnorderedHashNode<Key, Value>** buckets, size_t nbuckets)
	{
		size_t bucket = hash & (nbuckets - 1);

		UnorderedHashNode<Key, Value>* next = where->next;
		for (; buckets[bucket] == where; --bucket)
		{
			buckets[bucket] = next;
			if (!bucket)
				break;
		}

		if (where->prev)
			where->prev->next = where->next;
		if (next)
			next->prev = where->prev;
	}

	template<typename TNode>
	struct UnorderedHashIterator
	{
		TNode* operator->() const;

		TNode& operator*() const;

		TNode* node;
	};

	template<typename Node>
	struct UnorderedHashIterator<const Node>
	{

		UnorderedHashIterator()
		{}

		UnorderedHashIterator(UnorderedHashIterator<Node> other)
			: node(other.node)
		{
		}

		const Node* operator->() const;

		const Node& operator*() const;

		const Node* node;
	};

	template<typename Key>
	struct UnorderedHashIterator<const UnorderedHashNode<Key, void> >
	{
		const Key* operator->() const;

		const Key& operator*() const;

		UnorderedHashNode<Key, void>* node;
	};

	template<typename LNode, typename RNode>
	static inline bool operator==(const UnorderedHashIterator<LNode>& lhs, const UnorderedHashIterator<RNode>& rhs)
	{
		return lhs.node == rhs.node;
	}

	template<typename LNode, typename RNode>
	static inline bool operator!=(const UnorderedHashIterator<LNode>& lhs, const UnorderedHashIterator<RNode>& rhs)
	{
		return lhs.node != rhs.node;
	}

	template<typename Node>
	static inline void operator++(UnorderedHashIterator<Node>& lhs)
	{
		lhs.node = lhs.node->next;
	}

	template<typename Node>
	inline Node* UnorderedHashIterator<Node>::operator->() const
	{
		return node;
	}

	template<typename Node>
	inline Node& UnorderedHashIterator<Node>::operator*() const
	{
		return *node;
	}

	template<typename Node>
	inline const Node* UnorderedHashIterator<const Node>::operator->() const
	{
		return node;
	}

	template<typename Node>
	inline const Node& UnorderedHashIterator<const Node>::operator*() const
	{
		return *node;
	}

	template<typename Key>
	inline const Key* UnorderedHashIterator<const UnorderedHashNode<Key, void> >::operator->() const
	{
		return &node->first;
	}

	template<typename Key>
	inline const Key& UnorderedHashIterator<const UnorderedHashNode<Key, void> >::operator*() const
	{
		return node->first;
	}

	template<typename Node, typename Key>
	static inline Node unordered_hash_find(const Key& key, Node* buckets, size_t nbuckets)
	{
		if (!buckets) return 0;
		const size_t bucket = HashValue(key) & (nbuckets - 2);
		for (Node it = buckets[bucket], end = buckets[bucket + 1]; it != end; it = it->next)
			if (it->first == key)
				return it;

		return 0;
	}
}
#endif
