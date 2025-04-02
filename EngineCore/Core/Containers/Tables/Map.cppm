module;

#include "Core.h"
#include "Core/Utils/Logging/Logger.h"
#include "Core/Misc/Assertion.h"

export module Pawn.Core.Container.UnorderedMap;

import Pawn.Core.Memory.Allocator;
import Pawn.Core.Memory.Hasher;
import Pawn.Core.Misc.Pair;


export namespace Pawn::Core::Containers
{
	template<typename _Map>
	class UMapIterator
	{
	public:
		using NodeType = _Map::Node;
		using PtrType = NodeType*;
		using ValueType = _Map::ReturnType;

	public:
		UMapIterator(PtrType ptr, NodeType** buckets, SIZE_T num_buckets, SIZE_T bucket_index)
			: m_Ptr(ptr), m_Buckets(buckets), m_NumBuckets(num_buckets), m_BucketIndex(bucket_index) {
		}

		UMapIterator& operator++() {
			if (m_Ptr && m_Ptr->NextNode) {
				m_Ptr = m_Ptr->NextNode;
			}
			else {
				m_Ptr = nullptr;
				for (SIZE_T i = m_BucketIndex + 1; i < m_NumBuckets; ++i) {
					if (m_Buckets[i]) {
						m_Ptr = m_Buckets[i];
						m_BucketIndex = i;
						break;
					}
				}
			}
			return *this;
		}

		UMapIterator operator++(int) {
			UMapIterator temp = *this;
			++(*this);
			return temp;
		}

		bool operator!=(const UMapIterator& it) const {
			return m_Ptr != it.m_Ptr;
		}

		bool operator==(const UMapIterator& it) const {
			return m_Ptr == it.m_Ptr;
		}

		ValueType* operator->() {
			return reinterpret_cast<ValueType*>(&m_Ptr->Key);
		}

		ValueType& operator*() {
			return *reinterpret_cast<ValueType*>(&m_Ptr->Key);
		}

	private:
		PtrType m_Ptr;
		NodeType** m_Buckets;
		SIZE_T m_NumBuckets;
		SIZE_T m_BucketIndex;
	};

	template <typename keyType, typename valType, class _hasher = Core::Memory::Hasher<keyType>, class _allocator = Memory::Allocator<void>>
	class UnorderedMap
	{
	public:
		using KeyType = keyType;
		using ValueType = valType;

	private:
		struct Node {
			const KeyType Key;
			ValueType Value;
			Node* NextNode;

			Node(const KeyType& k, const ValueType& v) : Key(k), Value(v), NextNode(nullptr) {}
			Node(KeyType&& k, ValueType&& v) : Key(std::move(k)), Value(std::move(v)), NextNode(nullptr) {}
		};

	public:
		using ReturnType = Pawn::Core::Misc::Pair<KeyType, ValueType>;

		using HasherType = _hasher;

		using AllocatorType = _allocator;
		using NodeAllocator = AllocatorType::template Rebind<UnorderedMap::Node>::Other;

		using Iterator = UMapIterator<UnorderedMap>;

	public:
		UnorderedMap(SIZE_T initial_size = 16)
			: m_Buckets(nullptr), m_NumElements(0), m_NumBuckets(0), m_MaxLoadFactor(1.0f),
			m_Hash(HasherType()), m_NodeAllocator(NodeAllocator()) 
		{
			AllocateBuckets(initial_size);
		}

		UnorderedMap(const UnorderedMap& other)
			: m_Buckets(nullptr), m_NumElements(0), m_NumBuckets(0), m_MaxLoadFactor(other.m_MaxLoadFactor),
			m_Hash(other.m_Hash), m_NodeAllocator(other.m_NodeAllocator) 
		{
			AllocateBuckets(other.m_NumBuckets);

			for (SIZE_T i = 0; i < other.m_NumBuckets; ++i) 
			{
				Node* current = other.m_Buckets[i];
				Node** tail = &m_Buckets[i];
				while (current) 
				{
					Node* new_node = m_NodeAllocator.allocate(sizeof(Node));
					m_NodeAllocator.construct(new_node, current->key, current->value);
					*tail = new_node;
					tail = &new_node->next;
					current = current->next;
				}
			}
			m_NumElements = other.m_NumElements;
		}

		UnorderedMap(UnorderedMap&& other) noexcept
			: m_Buckets(other.m_Buckets), m_NumElements(other.m_NumElements), m_NumBuckets(other.m_NumBuckets),
			m_MaxLoadFactor(other.m_MaxLoadFactor), m_Hash(std::move(other.m_Hash)), m_NodeAllocator(std::move(other.m_NodeAllocator)) 
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
					Node* current = other.m_Buckets[i];
					Node** tail = &m_Buckets[i];
					while (current) {
						Node* new_node = m_NodeAllocator.Allocate(sizeof(Node));
						m_NodeAllocator.Construct(new_node, current->key, current->value);
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
		inline Iterator begin() {
			return Begin();
		}

		Iterator Begin()
		{
			for (SIZE_T i = 0; i < m_NumBuckets; ++i) {
				if (m_Buckets[i]) return Iterator(m_Buckets[i], m_Buckets, m_NumBuckets, i);
			}
			return Iterator(nullptr, m_Buckets, m_NumBuckets, m_NumBuckets);
		}

		inline Iterator End()
		{
			return Iterator(nullptr, m_Buckets, m_NumBuckets, m_NumBuckets);
		}

		inline Iterator end() {
			return End();
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

		inline ReturnType* Find(const KeyType& key) 
		{
			return PFind(key);
		}

		inline const ReturnType* Find(const KeyType& key) const
		{
			return PFind(key);
		}

		void Erase(const KeyType& key) {
			SIZE_T index = GetBucketIndex(key);
			Node* current = m_Buckets[index];
			Node* prev = nullptr;

			while (current) {
				if (current->key == key) 
				{
					if (prev)
						prev->next = current->next;
					else
						m_Buckets[index] = current->next;

					m_NodeAllocator.Destroy(current);
					m_NodeAllocator.Deallocate(current, sizeof(Node));
					m_NumElements--;
					return;
				}
				prev = current;
				current = current->next;
			}
		}

		inline SIZE_T Size() const
		{ 
			return m_NumElements;
		}

		inline void Clear() {
			ClearBuckets();
		}

	public:
		ValueType& operator[](const KeyType& key) {
			CheckAndRehash();

			SIZE_T index = GetBucketIndex(key);
			Node* current = m_Buckets[index];

			while (current) 
			{
				if constexpr (std::is_same<KeyType, const uchar*>::value) 
					if (strcmp(current->Key, key) == 0) return current->Value;
				else 
					if (current->Key == key) return current->Value;
				current = current->NextNode;
			}

			Node* newNode = m_NodeAllocator.Allocate(sizeof(Node));
			m_NodeAllocator.Construct(newNode, key, ValueType());
			newNode->NextNode = m_Buckets[index];
			m_Buckets[index] = newNode;
			m_NumElements++;
			return newNode->Value;
		}

	private:
		void PInsert(const KeyType& key, const ValueType& value)
		{
			CheckAndRehash();

			SIZE_T index = GetBucketIndex(key);
			Node* current = m_Buckets[index];

			while (current)
			{
				if (current->Key == key)
				{
					current->Value = value;
					return;
				}
				current = current->NextNode;
			}

			Node* newNode = m_NodeAllocator.Allocate(sizeof(Node));
			m_NodeAllocator.Construct(newNode, key, value);
			newNode->NextNode = m_Buckets[index];
			m_Buckets[index] = newNode;
			m_NumElements++;
		}

		void PInsert(KeyType&& key, ValueType&& value)
		{
			CheckAndRehash();

			SIZE_T index = GetBucketIndex(key);
			Node* current = m_Buckets[index];

			while (current)
			{
				if (current->Key == key)
				{
					current->Value = value;
					return;
				}
				current = current->NextNode;
			}

			Node* newNode = m_NodeAllocator.Allocate(sizeof(Node));
			m_NodeAllocator.Construct(newNode, std::move(key), std::move(value));
			newNode->NextNode = m_Buckets[index];
			m_Buckets[index] = newNode;
			m_NumElements++;
		}

		ReturnType* PFind(const KeyType& key)
		{
			SIZE_T index = GetBucketIndex(key);
			Node* current = m_Buckets[index];

			while (current) {
				if constexpr (std::is_same<KeyType, const char*>::value) 
					if (strcmp(current->Key, key) == 0) 
						return reinterpret_cast<const ReturnType*>(&current->Key);
				else 
					if (current->Key == key) 
						return reinterpret_cast<const ReturnType*>(&current->Key);
				current = current->NextNode;
			}
			return nullptr;
		}

	private:
		SIZE_T GetBucketIndex(const KeyType& key) const 
		{
			return m_Hash(key, m_NumBuckets);
		}

		void AllocateBuckets(SIZE_T newBucketsCount)
		{
			m_Buckets = reinterpret_cast<Node**>(m_NodeAllocator.Allocate(newBucketsCount * sizeof(Node*)));
			for (SIZE_T i = 0; i < newBucketsCount; ++i)
				m_Buckets[i] = nullptr;
			m_NumBuckets = newBucketsCount;
		}

		void DeallocateBuckets()
		{
			if (m_Buckets)
			{
				ClearBuckets();
				m_NodeAllocator.Deallocate(m_Buckets, m_NumBuckets * sizeof(Node*));
				m_Buckets = nullptr;
			}
		}

		void ClearBuckets()
		{
			for (SIZE_T i = 0; i < m_NumBuckets; ++i)
			{
				Node* current = m_Buckets[i];
				while (current) 
				{
					Node* next = current->NextNode;
					m_NodeAllocator.Destroy(current);
					m_NodeAllocator.Deallocate(current, sizeof(Node));
					current = next;
				}
				m_Buckets[i] = nullptr;
			}
			m_NumBuckets = 0;
		}

		void Rehash() 
		{
			SIZE_T newSize = m_NumBuckets * 2;
			Node** newBuckets = reinterpret_cast<Node**>(m_NodeAllocator.Allocate(newSize * sizeof(Node*)));
			for (SIZE_T i = 0; i < newSize; ++i) 
				newBuckets[i] = nullptr;

			for (SIZE_T i = 0; i < m_NumBuckets; ++i) 
			{
				Node* current = m_Buckets[i];
				while (current)
				{
					Node* next = current->NextNode;
					SIZE_T new_index = m_Hash(current->key, newSize);
					current->NextNode = newBuckets[new_index];
					newBuckets[new_index] = current;
					current = next;
				}
			}

			m_NodeAllocator.Deallocate(reinterpret_cast<void*>(m_Buckets), m_NumBuckets * sizeof(Node*));
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
		Node** m_Buckets;
		SIZE_T m_NumElements;
		SIZE_T m_NumBuckets;

	private:
		NodeAllocator m_NodeAllocator;
		HasherType m_Hash;

	};

}
