#pragma once

#include "Core.hpp"
#include "Core/Misc/Pair.hpp"
#include "Core/Memory/Hasher.hpp"
#include "Core/Memory/Allocators/Allocator.hpp"

#include <unordered_set>

namespace ME::Core::Containers
{
	template <typename ValueType>
	struct SetNode
	{
		const ValueType Value;
		SetNode* NextNode;

		SetNode(const ValueType& k) : Value(k), NextNode(nullptr)
		{
		}

		SetNode(ValueType&& k) : Value(std::move(k)), NextNode(nullptr)
		{
		}
	};

	template <typename _Set>
	class USetIterator
	{
	public:
		using NodeType = typename _Set::NodeType;
		using PtrType = NodeType*;
		using ValueType = typename _Set::ReturnType;

	public:
		USetIterator(PtrType ptr, NodeType** buckets, SIZE_T num_buckets, SIZE_T bucket_index)
			: m_Ptr(ptr), m_Buckets(buckets), m_NumBuckets(num_buckets), m_BucketIndex(bucket_index), m_Value(m_Ptr->Value)
		{
		}

		USetIterator& operator++()
		{
			if (m_Ptr && m_Ptr->NextNode)
				m_Ptr = m_Ptr->NextNode;
			else
			{
				m_Ptr = nullptr;
				for (SIZE_T i = m_BucketIndex + 1; i < m_NumBuckets; ++i)
				{
					if (m_Buckets[i])
					{
						m_Ptr = m_Buckets[i];
						m_BucketIndex = i;
						return *this;
					}
				}
				m_BucketIndex = m_NumBuckets;
			}
			return *this;
		}

		USetIterator operator++(int)
		{
			USetIterator temp = *this;
			++(*this);
			return temp;
		}

		inline bool operator!=(const USetIterator& it) const
		{
			return m_Ptr != it.m_Ptr;
		}

		inline bool operator==(const USetIterator& it) const
		{
			return m_Ptr == it.m_Ptr;
		}

		ValueType* operator->()
		{
			return &m_Ptr->Value;
		}

		inline ValueType& operator*()
		{
			return m_Ptr->Value;
		}

	private:
		PtrType m_Ptr;
		NodeType** m_Buckets;
		SIZE_T m_NumBuckets;
		SIZE_T m_BucketIndex;
	};

	template <typename valueType, class _hasher = Core::Memory::Hasher<valueType>, class _allocator =
	          Memory::Allocator<void>>
	class UnorderedSet
	{
	public:
		using ValueType = valueType;

		using NodeType = SetNode<ValueType>;

	public:
		using HasherType = _hasher;

		using AllocatorType = _allocator;
		using NodeAllocator = AllocatorType::template Rebind<NodeType>::Other;
		using BucketAllocator = AllocatorType::template Rebind<NodeType*>::Other;

		using Iterator = USetIterator<UnorderedSet>;

	public:
		UnorderedSet(SIZE_T initial_size = 16)
			: m_MaxLoadFactor(1.0f), m_Buckets(nullptr), m_NumElements(0), m_NumBuckets(0),
			  m_NodeAllocator(NodeAllocator()), m_Hash(HasherType())
		{
			AllocateBuckets(initial_size);
		}

		UnorderedSet(const UnorderedSet& other)
			: m_MaxLoadFactor(other.m_MaxLoadFactor), m_Buckets(nullptr), m_NumElements(0), m_NumBuckets(0),
			  m_NodeAllocator(other.m_NodeAllocator), m_Hash(other.m_Hash)
		{
			AllocateBuckets(other.m_NumBuckets);

			for (SIZE_T i = 0; i < other.m_NumBuckets; ++i)
			{
				NodeType* current = other.m_Buckets[i];
				NodeType** tail = &m_Buckets[i];
				while (current)
				{
					NodeType* new_node = m_NodeAllocator.Allocate(sizeof(NodeType));
					m_NodeAllocator.Construct(new_node, current->Value);
					*tail = new_node;
					tail = &new_node->NextNode;
					current = current->NextNode;
				}
			}
			m_NumElements = other.m_NumElements;
		}

		UnorderedSet(UnorderedSet&& other) noexcept
			: m_MaxLoadFactor(std::move(other.m_MaxLoadFactor)), m_Buckets(std::move(other.m_Buckets)), m_NumElements(std::move(other.m_NumElements)),
			  m_NumBuckets(std::move(other.m_NumBuckets)), m_NodeAllocator(std::move(other.m_NodeAllocator)),
			  m_Hash(std::move(other.m_Hash))
		{
			other.m_Buckets = nullptr;
			other.m_NumElements = 0;
			other.m_NumBuckets = 0;
		}

		~UnorderedSet()
		{
			DeallocateBuckets();
		}

	public:
		UnorderedSet& operator=(const UnorderedSet& other)
		{
			if (this != &other)
			{
				DeallocateBuckets();
				m_Hash = other.m_Hash;
				m_NodeAllocator = other.m_NodeAllocator;
				m_MaxLoadFactor = other.m_MaxLoadFactor;
				AllocateBuckets(other.m_NumBuckets);
				for (SIZE_T i = 0; i < other.m_NumBuckets; ++i)
				{
					NodeType* current = other.m_Buckets[i];
					NodeType** tail = &m_Buckets[i];
					while (current)
					{
						NodeType* new_node = m_NodeAllocator.Allocate(sizeof(SetNode));
						m_NodeAllocator.Construct(new_node, current->Value);
						*tail = new_node;
						tail = &new_node->NextNode;
						current = current->NextNode;
					}
				}
				m_NumElements = other.m_NumElements;
			}
			return *this;
		}

		UnorderedSet& operator=(UnorderedSet&& other) noexcept
		{
			if (this != &other)
			{
				DeallocateBuckets();

				m_Buckets = other.m_Buckets;
				m_NumElements = other.m_NumElements;
				m_NumBuckets = other.m_NumBuckets;
				m_MaxLoadFactor = other.m_MaxLoadFactor;
				m_Hash = std::move(other.m_Hash);
				m_NodeAllocator = std::move(other.m_NodeAllocator);

				other.m_Buckets = nullptr;
				other.m_NumElements = 0;
				other.m_NumBuckets = 0;
			}
			return *this;
		}

	public:
		inline Iterator begin()
		{
			return Begin();
		}

		Iterator Begin()
		{
			for (SIZE_T i = 0; i < m_NumBuckets; ++i)
			{
				if (m_Buckets[i]) return Iterator(m_Buckets[i], m_Buckets, m_NumBuckets, i);
			}
			return Iterator(nullptr, m_Buckets, m_NumBuckets, m_NumBuckets);
		}

		inline Iterator End()
		{
			return Iterator(nullptr, m_Buckets, m_NumBuckets, m_NumBuckets);
		}

		inline Iterator end()
		{
			return End();
		}

	public:
		inline void Insert(const ValueType& value)
		{
			PInsert(value);
		}

		inline void Insert(ValueType&& value)
		{
			PInsert(std::move(value));
		}

		inline Iterator Find(const ValueType& value)
		{
			return PFind(value);
		}

		inline Iterator Find(const ValueType& value) const
		{
			return PFind(value);
		}

		void Erase(const ValueType& value)
		{
			SIZE_T index = GetBucketIndex(value);
			NodeType* current = m_Buckets[index];
			NodeType* prev = nullptr;

			while (current)
			{
				if (current->Value == value)
				{
					if (prev)
						prev->NextNode = current->NextNode;
					else
						m_Buckets[index] = current->NextNode;

					m_NodeAllocator.Destroy(current);
					m_NodeAllocator.Deallocate(current, sizeof(NodeType));
					m_NumElements--;
					return;
				}
				prev = current;
				current = current->NextNode;
			}
		}

		void Erase(Iterator& iterator)
		{
			ValueType value = *iterator;

			SIZE_T index = GetBucketIndex(value);
			NodeType* current = m_Buckets[index];
			NodeType* prev = nullptr;

			while (current)
			{
				if (current->Value == value)
				{
					if (prev)
						prev->NextNode = current->NextNode;
					else
						m_Buckets[index] = current->NextNode;

					m_NodeAllocator.Destroy(current);
					m_NodeAllocator.Deallocate(current, sizeof(NodeType));
					m_NumElements--;
					return;
				}
				prev = current;
				current = current->NextNode;
			}
		}

		inline SIZE_T Size() const
		{
			return m_NumElements;
		}

		inline void Clear()
		{
			ClearBuckets();
		}

		inline bool Reserve(SIZE_T capacity)
		{
			return PReserve(capacity);
		}

	private:
		void PInsert(const ValueType& value)
		{
			CheckAndRehash();

			SIZE_T index = GetBucketIndex(value);
			NodeType* current = m_Buckets[index];

			while (current)
			{
				if (current->Value == value)
					return;
				current = current->NextNode;
			}

			NodeType* newNode = m_NodeAllocator.Allocate(sizeof(NodeType));
			m_NodeAllocator.Construct(newNode, value);
			newNode->NextNode = m_Buckets[index];
			m_Buckets[index] = newNode;
			m_NumElements++;
		}

		void PInsert(ValueType&& value)
		{
			CheckAndRehash();

			SIZE_T index = GetBucketIndex(value);
			NodeType* current = m_Buckets[index];

			while (current)
			{
				if (current->Value == value)
					return;
				current = current->NextNode;
			}

			NodeType* newNode = m_NodeAllocator.Allocate(sizeof(NodeType));
			m_NodeAllocator.Construct(newNode, std::move(value));
			newNode->NextNode = m_Buckets[index];
			m_Buckets[index] = newNode;
			m_NumElements++;
		}

		Iterator PFind(const ValueType& value)
		{
			if (m_NumElements <= 0) return End();
			SIZE_T index = GetBucketIndex(value);
			NodeType* current = m_Buckets[index];

			while (current)
			{
				if (current->Value == value)
					return Iterator(current, m_Buckets, m_NumBuckets, index);
				current = current->NextNode;
			}
			return End();
		}

		bool PReserve(SIZE_T capacity)
		{
			if (capacity <= m_NumBuckets)
				return false;

			Rehash(capacity);

			return true;
		}

	private:
		SIZE_T GetBucketIndex(const ValueType& value) const
		{
			return m_Hash(value, m_NumBuckets);
		}

		void AllocateBuckets(SIZE_T newBucketsCount)
		{
			m_Buckets = reinterpret_cast<NodeType**>(m_BucketAllocator.Allocate(newBucketsCount * sizeof(NodeType*)));
			for (SIZE_T i = 0; i < newBucketsCount; ++i)
				m_Buckets[i] = nullptr;
			m_NumBuckets = newBucketsCount;
		}

		void DeallocateBuckets()
		{
			if (m_Buckets)
			{
				ClearBuckets();
				m_BucketAllocator.Deallocate(m_Buckets, m_NumBuckets * sizeof(NodeType*));
				m_Buckets = nullptr;
			}
		}

		void ClearBuckets()
		{
			for (SIZE_T i = 0; i < m_NumBuckets; ++i)
			{
				NodeType* current = m_Buckets[i];
				while (current)
				{
					NodeType* next = current->NextNode;
					m_NodeAllocator.Destroy(current);
					m_NodeAllocator.Deallocate(current, sizeof(NodeType));
					current = next;
				}
				m_Buckets[i] = nullptr;
			}
			m_NumElements = 0;
		}

		void Rehash()
		{
			SIZE_T newSize = m_NumBuckets * 2;
			NodeType** newBuckets = reinterpret_cast<NodeType**>(m_BucketAllocator.
				Allocate(newSize * sizeof(NodeType*)));
			for (SIZE_T i = 0; i < newSize; ++i)
				newBuckets[i] = nullptr;

			for (SIZE_T i = 0; i < m_NumBuckets; ++i)
			{
				NodeType* current = m_Buckets[i];
				while (current)
				{
					NodeType* next = current->NextNode;
					SIZE_T new_index = m_Hash(current->Value, newSize);
					current->NextNode = newBuckets[new_index];
					newBuckets[new_index] = current;
					current = next;
				}
			}

			m_BucketAllocator.Deallocate(m_Buckets, m_NumBuckets * sizeof(NodeType*));
			m_Buckets = newBuckets;
			m_NumBuckets = newSize;
		}

		void Rehash(SIZE_T numBuckets)
		{
			SIZE_T newSize = numBuckets;
			NodeType** newBuckets = reinterpret_cast<NodeType**>(m_BucketAllocator.
				Allocate(newSize * sizeof(NodeType*)));
			for (SIZE_T i = 0; i < newSize; ++i)
				newBuckets[i] = nullptr;

			for (SIZE_T i = 0; i < m_NumBuckets; ++i)
			{
				NodeType* current = m_Buckets[i];
				while (current)
				{
					NodeType* next = current->NextNode;
					SIZE_T new_index = m_Hash(current->Value, newSize);
					current->NextNode = newBuckets[new_index];
					newBuckets[new_index] = current;
					current = next;
				}
			}

			m_BucketAllocator.Deallocate(m_Buckets, m_NumBuckets * sizeof(NodeType*));
			m_Buckets = newBuckets;
			m_NumBuckets = newSize;
		}

		void CheckAndRehash()
		{
			if (static_cast<float>(m_NumElements + 1) / m_NumBuckets > m_MaxLoadFactor)
				Rehash();
		}

	private:
		float m_MaxLoadFactor;
		NodeType** m_Buckets;
		SIZE_T m_NumElements;
		SIZE_T m_NumBuckets;

	private:
		NodeAllocator m_NodeAllocator;
		BucketAllocator m_BucketAllocator;
		HasherType m_Hash;
	};
}
