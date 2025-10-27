#pragma once

#include "Core.hpp"
#include "Core/Misc/Pair.hpp"
#include "Core/Memory/Hasher.hpp"
#include "Core/Memory/Allocators/Allocator.hpp"

namespace ME::Core::Containers
{
	template <typename KeyType, typename ValueType>
	struct MapNode
	{
	    ME::Core::Misc::Pair<KeyType, ValueType> Pair;
	    KeyType& Key;
	    ValueType& Value;
		MapNode* NextNode;

		MapNode(const KeyType& k, const ValueType& v)
	        : Pair({ k, v }), Key(Pair.Value1), Value(Pair.Value2), NextNode(nullptr) {}

		MapNode(KeyType&& k, ValueType&& v)
	        : Pair({ std::move(k), std::move(v) }), Key(Pair.Value1), Value(Pair.Value2), NextNode(nullptr) {}

		ME::Core::Misc::Pair<KeyType, ValueType>& ToPair() { return Pair; }
	};

	template <typename _Map>
	class UMapIterator
	{
	public:
		using NodeType = typename _Map::NodeType;
		using PtrType = NodeType*;
		using ValueType = typename _Map::ReturnType;

	public:
		UMapIterator(PtrType ptr, NodeType** buckets, SIZE_T num_buckets, SIZE_T bucket_index)
			: m_Ptr(ptr), m_Buckets(buckets), m_NumBuckets(num_buckets), m_BucketIndex(bucket_index), m_Pair(ValueType(m_Ptr->Key, m_Ptr->Value)) {}

		UMapIterator& operator++()
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
						break;
					}
				}
			}
			return *this;
		}

		UMapIterator operator++(int)
		{
			UMapIterator temp = *this;
			++(*this);
			return temp;
		}

		inline bool operator!=(const UMapIterator& it) const
		{
			return m_Ptr != it.m_Ptr;
		}

		inline bool operator==(const UMapIterator& it) const
		{
			return m_Ptr == it.m_Ptr;
		}

		ValueType* operator->()
		{
			return reinterpret_cast<ValueType*>(&m_Pair);
		}

		inline ValueType& operator*()
		{
			return m_Ptr->Pair;
		}

	private:
		PtrType m_Ptr;
		NodeType** m_Buckets;
		SIZE_T m_NumBuckets;
		SIZE_T m_BucketIndex;
		ValueType m_Pair;
	};

	template <typename _Map>
	class UMapConstIterator
	{
	public:
		using NodeType = typename _Map::NodeType;
		using PtrType = NodeType*;
		using ValueType = typename _Map::ReturnType;

	public:
		UMapConstIterator(PtrType ptr, NodeType** buckets, SIZE_T num_buckets, SIZE_T bucket_index)
			: m_Ptr(ptr), m_Buckets(buckets), m_NumBuckets(num_buckets), m_BucketIndex(bucket_index) {}

		UMapConstIterator& operator++()
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
						break;
					}
				}
			}
			return *this;
		}

		UMapConstIterator operator++(int)
		{
			UMapConstIterator temp = *this;
			++(*this);
			return temp;
		}

		inline bool operator!=(const UMapConstIterator& it) const
		{
			return m_Ptr != it.m_Ptr;
		}

		inline bool operator==(const UMapConstIterator& it) const
		{
			return m_Ptr == it.m_Ptr;
		}

		const ValueType* operator->() const
		{
			return &m_Ptr->Pair;
		}

		inline const ValueType& operator*() const
		{
			return m_Ptr->Pair;
		}

	private:
	    PtrType m_Ptr;
		NodeType** m_Buckets;
		SIZE_T m_NumBuckets;
		SIZE_T m_BucketIndex;
	};

	template <typename keyType, typename valType, class _hasher = Core::Memory::Hasher<keyType>, class _allocator =
	          Memory::Allocator<void>>
	class UnorderedMap
	{
	public:
		using KeyType = keyType;
		using ValueType = valType;

		using NodeType = MapNode<KeyType, ValueType>;

	public:
		using ReturnType = ME::Core::Misc::Pair<KeyType, ValueType>;

		using HasherType = _hasher;

		using AllocatorType = _allocator;
		using NodeAllocator = AllocatorType::template Rebind<NodeType>::Other;
		using BucketAllocator = AllocatorType::template Rebind<NodeType*>::Other;

		using Iterator = UMapIterator<UnorderedMap>;
		using ConstIterator = UMapIterator<UnorderedMap>;

	public:
		UnorderedMap(SIZE_T initial_size = 16)
			: m_MaxLoadFactor(1.0f), m_Buckets(nullptr), m_NumElements(0), m_NumBuckets(0),
			  m_NodeAllocator(NodeAllocator()), m_Hash(HasherType())
		{
			AllocateBuckets(initial_size);
		}

		UnorderedMap(const UnorderedMap& other)
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
					m_NodeAllocator.Construct(new_node, current->Key, current->Value);
					*tail = new_node;
					tail = &new_node->NextNode;
					current = current->NextNode;
				}
			}
			m_NumElements = other.m_NumElements;
		}

		UnorderedMap(UnorderedMap&& other) noexcept
			: m_MaxLoadFactor(other.m_MaxLoadFactor), m_Buckets(other.m_Buckets), m_NumElements(other.m_NumElements),
			  m_NumBuckets(other.m_NumBuckets), m_NodeAllocator(std::move(other.m_NodeAllocator)),
			  m_Hash(std::move(other.m_Hash))
		{
			other.m_Buckets = nullptr;
			other.m_NumElements = 0;
			other.m_NumBuckets = 0;
		}

		~UnorderedMap()
		{
			DeallocateBuckets();
		}

	public:
		UnorderedMap& operator=(const UnorderedMap& other)
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
						NodeType* new_node = m_NodeAllocator.Allocate(sizeof(NodeType));
						m_NodeAllocator.Construct(new_node, current->Key, current->Value);
						*tail = new_node;
						tail = &new_node->NextNode;
						current = current->NextNode;
					}
				}
				m_NumElements = other.m_NumElements;
			}
			return *this;
		}

		UnorderedMap& operator=(UnorderedMap&& other) noexcept
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

		inline Iterator cbegin() const
		{
			return CBegin();
		}

		ConstIterator CBegin() const
		{
			for (SIZE_T i = 0; i < m_NumBuckets; ++i)
			{
				if (m_Buckets[i]) return ConstIterator(m_Buckets[i], m_Buckets, m_NumBuckets, i);
			}
			return ConstIterator(nullptr, m_Buckets, m_NumBuckets, m_NumBuckets);
		}

		inline Iterator end()
		{
			return End();
		}

		inline Iterator End()
		{
			return Iterator(nullptr, m_Buckets, m_NumBuckets, m_NumBuckets);
		}

		inline Iterator cend() const
		{
			return CEnd();
		}

		inline Iterator CEnd() const
		{
			return Iterator(nullptr, m_Buckets, m_NumBuckets, m_NumBuckets);
		}

	public:
		inline void Insert(const KeyType& key, const ValueType& value)
		{
			PInsert(key, value);
		}

		inline void Insert(KeyType&& key, ValueType&& value)
		{
			PInsert(std::move(key), std::move(value));
		}

		inline Iterator Find(const KeyType& key)
		{
			return PFind(key);
		}

		inline Iterator Find(const KeyType& key) const
		{
			return PFind(key);
		}

		void Erase(const KeyType& key)
		{
			SIZE_T index = GetBucketIndex(key);
			NodeType* current = m_Buckets[index];
			NodeType* prev = nullptr;

			while (current)
			{
				if (current->Key == key)
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
			KeyType key = iterator->Value1;

			SIZE_T index = GetBucketIndex(key);
			NodeType* current = m_Buckets[index];
			NodeType* prev = nullptr;

			while (current)
			{
				if (current->Key == key)
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

		inline bool Contains(const KeyType& key) const
		{
		    return PFind(key) != CEnd();
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

	public:
		ValueType& operator[](const KeyType& key)
		{
			CheckAndRehash();

			SIZE_T index = GetBucketIndex(key);
			NodeType* current = m_Buckets[index];

			while (current)
			{
				if constexpr (std::is_same_v<KeyType, const asciichar*> || std::is_same_v<KeyType, const char8*>)
				{
					if (strcmp(reinterpret_cast<const asciichar*>(current->Key), key) == 0)
					{
						return current->Value;
					}
				}
				else if constexpr (std::is_same_v<KeyType, const wchar*>)
				{
					if (wcscmp(current->Key, key) == 0)
					{
						return current->Value;
					}
				}
				else if (current->Key == key) return current->Value;
				{
					return current->Value;
				}
				current = current->NextNode;
			}

			NodeType* newNode = m_NodeAllocator.Allocate(sizeof(NodeType));
			new(newNode) NodeType(key, ValueType());
			newNode->NextNode = m_Buckets[index];
			m_Buckets[index] = newNode;
			m_NumElements++;
			return newNode->Value;
		}

		const ValueType& operator[](const KeyType& key) const
		{
			SIZE_T index = GetBucketIndex(key);
			NodeType* current = m_Buckets[index];
			while (current)
			{
				if constexpr (std::is_same_v<KeyType, const asciichar*> || std::is_same_v<KeyType, const char8*>)
				{
					if (strcmp(reinterpret_cast<const asciichar*>(current->Key), key) == 0)
						return current->Value;
				}
				else if constexpr (std::is_same_v<KeyType, const wchar*>)
				{
					if (wcscmp(current->Key, key) == 0)
						return current->Value;
				}
				else if (current->Key == key)
				{
					return current->Value;
				}
				current = current->NextNode;
			}
		}

	private:
		void PInsert(const KeyType& key, const ValueType& value)
		{
			CheckAndRehash();

			SIZE_T index = GetBucketIndex(key);
			NodeType* current = m_Buckets[index];

			while (current)
			{
				if (current->Key == key)
				{
					current->Value = value;
					return;
				}
				current = current->NextNode;
			}

			NodeType* newNode = m_NodeAllocator.Allocate(sizeof(NodeType));
			m_NodeAllocator.Construct(newNode, key, value);
			newNode->NextNode = m_Buckets[index];
			m_Buckets[index] = newNode;
			m_NumElements++;
		}

		void PInsert(KeyType&& key, ValueType&& value)
		{
			CheckAndRehash();

			SIZE_T index = GetBucketIndex(key);
			NodeType* current = m_Buckets[index];

			while (current)
			{
				if (current->Key == key)
				{
					current->Value = value;
					return;
				}
				current = current->NextNode;
			}

			NodeType* newNode = m_NodeAllocator.Allocate(sizeof(NodeType));
			m_NodeAllocator.Construct(newNode, std::move(key), std::move(value));
			newNode->NextNode = m_Buckets[index];
			m_Buckets[index] = newNode;
			m_NumElements++;
		}

		Iterator PFind(const KeyType& key)
		{
			if (m_NumElements <= 0) return End();
			SIZE_T index = GetBucketIndex(key);
			NodeType* current = m_Buckets[index];

			while (current)
			{
				if (current->Key == key)
					return Iterator(current, m_Buckets, m_NumBuckets, index);
				current = current->NextNode;
			}
			return End();
		}

		ConstIterator PFind(const KeyType& key) const
		{
			if (m_NumElements <= 0) return CEnd();
			SIZE_T index = GetBucketIndex(key);
			NodeType* current = m_Buckets[index];

			while (current)
			{
				if (current->Key == key)
					return ConstIterator(current, m_Buckets, m_NumBuckets, index);
				current = current->NextNode;
			}
			return CEnd();
		}

		bool PReserve(SIZE_T capacity)
		{
			if (capacity <= m_NumBuckets)
				return false;

			Rehash(capacity);

			return true;
		}

	private:
		SIZE_T GetBucketIndex(const KeyType& key) const
		{
			return m_Hash(key, m_NumBuckets);
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
					SIZE_T new_index = m_Hash(current->Key, newSize);
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
					SIZE_T new_index = m_Hash(current->Key, newSize);
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
