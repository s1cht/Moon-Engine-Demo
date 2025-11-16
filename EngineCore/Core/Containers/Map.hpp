#pragma once
#include "Core.hpp"
#include "Core/Pair.hpp"
#include "Core/Memory/Allocators/Allocator.hpp"
#include "Core/Algorithm.hpp"
#include "Core/Utility/Logging/Logger.hpp"

#define ME_MAP_ITER_CHECK(ret) if (this->m_Ptr == nullptr)			\
{																	\
    ME_ASSERT(false, "Can't iterate with ptr = nullptr!");			\
    return ret;														\
}

namespace ME::Core
{
	template <typename KeyType, typename ValType>
	struct MapBranch
	{
		// String
		Misc::Pair<KeyType, ValType> Pair;
		uint32 Height;
		// Pointers
		MapBranch* Parent;
		MapBranch* LeftPart;
		MapBranch* RightPart;

		MapBranch* Next()
		{
			MapBranch* next = nullptr;
			if (RightPart == nullptr)
			{
				next = this;
				while (next->Parent != nullptr && next == next->Parent->RightPart)
					next = next->Parent;
				next = next->Parent;
			}
			else
			{
				next = RightPart;
				while (next->LeftPart != nullptr)
					next = next->LeftPart;
			}

			return next;
		}

		MapBranch* Last()
		{
			MapBranch* last = nullptr;
			if (LeftPart == nullptr)
			{
				last = this;
				while (last->Parent != nullptr && last == last->Parent->LeftPart)
					last = last->Parent;
				last = last->Parent;
			}
			else
			{
				last = LeftPart;
				while (last->RightPart != nullptr)
					last = last->RightPart;
			}
			return last;
		}
	};

	template<typename _Map>
	class MapIterator
	{
	public:
		using DataType = typename _Map::PairType;
		using PtrType = _Map::BranchType*;
		using RefType = DataType&;

	public:
		MapIterator() : m_Ptr(nullptr) {}
		MapIterator(PtrType ptr) : m_Ptr(ptr) {}

	public:

		MapIterator operator-(const SIZE_T& index)
		{
			ME_MAP_ITER_CHECK(MapIterator(nullptr));
			PtrType temp = m_Ptr;
			for (SIZE_T i = 0; i < index; i++)
				temp = temp->Next();
		    return MapIterator(temp);
		}

		MapIterator operator+(const SIZE_T& index)
		{
			ME_MAP_ITER_CHECK(MapIterator(nullptr));
			PtrType temp = m_Ptr;
			for (SIZE_T i = 0; i < index; i++)
				temp = temp->Last();
		    return MapIterator(temp);
		}

		void operator-=(const SIZE_T& index)
		{
			this->m_Ptr -= index;
		}

		void operator+=(const SIZE_T& index)
		{
			PtrType temp = m_Ptr;
			for (SIZE_T i = 0; i < index; i++)
				temp = temp->Next();

		    this->m_Ptr = temp;
		}

		MapIterator& operator++()
		{
		    this->operator+=(1);
			return (*this);
		}

		MapIterator operator++(int)
		{
			MapIterator temp = (*this);
			this->operator+=(1);
			return temp;
		}

		MapIterator& operator--()
		{
			ME_MAP_ITER_CHECK(MapIterator(nullptr));
			this->operator-=(1);
			return (*this);
		}

		MapIterator operator--(int)
		{
			ME_MAP_ITER_CHECK(MapIterator(nullptr));
			MapIterator temp = (*this);
			this->operator-=(1);
			return temp;
		}

		bool operator!=(const MapIterator& it)
		{
			return (m_Ptr != it.m_Ptr);
		}

		bool operator==(const MapIterator& it)
		{
			return (m_Ptr == it.m_Ptr);
		}

		MapIterator& operator= (const MapIterator& it)
		{
			this->m_Ptr = it.m_Ptr;
			return *this;
		}

		RefType operator[] (SIZE_T index)
		{
			ME_MAP_ITER_CHECK(MapIterator(nullptr));
		    return this->operator+(index).m_Ptr->Pair;
		}

		DataType* operator->()
		{
			return &m_Ptr->Pair;
		}

		RefType operator*()
		{
			return m_Ptr->Pair;
		}

	public:
		PtrType m_Ptr;
	};

	template <typename keyType, typename valType, class _allocator =
		Memory::Allocator<MapBranch<keyType, valType>>>
    class Map
	{
	public:
		using KeyType = keyType;
		using ValueType = valType;

		using PairType = ME::Core::Misc::Pair<KeyType, ValueType>;
		using BranchType = MapBranch<KeyType, ValueType>;
		using AllocatorType = _allocator;

		using Iterator = MapIterator<Map>;

	public:
		constexpr Map() : m_Root(nullptr), m_ElementCount(0), m_Allocator(AllocatorType()) {}

	    Map(const Map& other) noexcept
		{
			m_ElementCount = other.m_ElementCount;
			m_Allocator = other.m_Allocator;
			m_Root = CloneBranch(other.m_Root);
		}

	    Map(Map&& other) noexcept
		{
			m_Allocator = other.m_Allocator;
			m_ElementCount = other.m_ElementCount;
			m_Root = other.m_Root;

			other.m_ElementCount = 0;
			other.m_Root = nullptr;
		}

		~Map()
		{
			m_ElementCount = 0;
			Clear();
		}

	public:
		inline ValueType& operator[](const KeyType& key) noexcept
		{
			BranchType* node = SearchBranch(key);
			if (node) return node->Pair.Value2;

			m_Root = PInsert(m_Root, key, ValueType());
			++m_ElementCount;
			return SearchBranch(key)->Pair.Value2;
		}

	public:
		inline void Insert(const KeyType& key, const ValueType& value)
		{
			m_Root = PInsert(m_Root, key, value);
		}

		inline void Insert(KeyType&& key, ValueType&& value)
		{
			m_Root = PInsert(m_Root, std::move(key), std::move(value));
		}

		inline void Erase(const KeyType& key)
		{
			m_Root = PErase(m_Root, key);
		}

		inline ValueType& At(const KeyType& key)
		{
			BranchType* node = SearchBranch(key);
			ME_ASSERT(node != nullptr, "Key not found in Map");
			return node->Pair.Value2;
		}

		inline const ValueType& At(const KeyType& key) const
		{
			const BranchType* node = SearchBranch(key);
			ME_ASSERT(node != nullptr, "Key not found in Map");
			return node->Pair.Value2;
		}

		inline void Clear()
		{
			PClear(m_Root);
			m_Root = nullptr;
			m_ElementCount = 0;
		}

		inline Iterator Find(const KeyType& key)
		{
			return Iterator(SearchBranch(key));
		}

		ME_NODISCARD inline SIZE_T Size() const
		{
			return m_ElementCount;
		}

		ME_NODISCARD inline bool Empty() const
		{
			return m_Root == nullptr;
		}

		ME_NODISCARD inline const BranchType* Data() const
		{
			return m_Root;
		}

		inline Iterator begin()
		{
			return Begin();
		}

		inline Iterator end()
		{
			return End();
		}

		inline Iterator Begin()
		{
			return Iterator(GetLeftmostBranch());
		}

		inline Iterator End()
		{
			return Iterator(nullptr);
		}

		ME_NODISCARD inline PairType& Front()
		{
			return GetLeftmostBranch()->Pair;
		}

		ME_NODISCARD inline PairType& Back()
		{
			return GetRightmostBranch()->Pair;
		}

	private:
		BranchType* PInsert(BranchType* root, const KeyType& key, const ValueType& value)
		{
			if (root == nullptr)
			{
				m_ElementCount++;
				return AllocateBranch(key, value);
			}
			else if (key < root->Pair.Value1)
			{
				root->LeftPart = PInsert(root->LeftPart, key, value);
				if (root->LeftPart) root->LeftPart->Parent = root;
			}
			else if (key > root->Pair.Value1)
			{
				root->RightPart = PInsert(root->RightPart, key, value);
				if (root->RightPart) root->RightPart->Parent = root;
			}
			else
			{
				root->Pair.Value2 = value;
				return root;
			}

			root->Height = 1 + ME::Core::Algorithm::Max(GetHeight(root->LeftPart), GetHeight(root->RightPart));
			int32 balanceFactor = GetBalanceFactor(root);

			if (balanceFactor > 1 && key < root->LeftPart->Pair.Value1)
				return RotateRight(root);
			if (balanceFactor < -1 && key > root->RightPart->Pair.Value1)
				return RotateLeft(root);
			if (balanceFactor > 1 && key > root->LeftPart->Pair.Value1)
			{
				root->LeftPart = RotateLeft(root->LeftPart);
				if (root->LeftPart) root->LeftPart->Parent = root;
				return RotateRight(root);
			}
			if (balanceFactor < -1 && key < root->RightPart->Pair.Value1)
			{
				root->RightPart = RotateRight(root->RightPart);
				if (root->RightPart) root->RightPart->Parent = root;
				return RotateLeft(root);
			}

			return root;
		}


		BranchType* PInsert(BranchType* root, KeyType&& key, ValueType&& value)
		{
			if (root == nullptr)
			{
				m_ElementCount++;
				return AllocateBranch(key, value);
			}
			else if (key < root->Pair.Value1)
			{
				root->LeftPart = PInsert(root->LeftPart, std::move(key), std::move(value));
				root->LeftPart->Parent = root;
			}
			else if (key > root->Pair.Value1)
			{
				root->RightPart = PInsert(root->RightPart, key, value);
				if (root->RightPart) root->RightPart->Parent = root;
			}
			else
			{
				root->Pair.Value2 = std::move(value);
				return root;
			}

			root->Height = 1 + ME::Core::Algorithm::Max(GetHeight(root->LeftPart), GetHeight(root->RightPart));

			int32 balanceFactor = GetBalanceFactor(root);

			if (balanceFactor > 1 && key < root->LeftPart->Pair.Value1)
				return RotateRight(root);
			if (balanceFactor < -1 && key > root->RightPart->Pair.Value1)
				return RotateLeft(root);
			if (balanceFactor > 1 && key > root->LeftPart->Pair.Value1)
			{
				root->LeftPart = RotateLeft(root->LeftPart);
				return RotateRight(root);
			}
			if (balanceFactor < -1 && key < root->RightPart->Pair.Value1)
			{
				root->RightPart = RotateRight(root->RightPart);
				return RotateLeft(root);
			}

			return root;
		}

		BranchType* PErase(BranchType* head, const KeyType& key)
		{
			if (head == nullptr)
				return head;

			if (key < head->Pair.Value1)
			{
				head->LeftPart = PErase(head->LeftPart, key);
				if (head->LeftPart) head->LeftPart->Parent = head;
			}
			else if (key > head->Pair.Value1)
			{
				head->RightPart = PErase(head->RightPart, key);
				if (head->RightPart) head->RightPart->Parent = head;
			}
			else
			{
				if (head->LeftPart == nullptr || head->RightPart == nullptr)
				{
					BranchType* temp = head->LeftPart ? head->LeftPart : head->RightPart;
					DeallocateBranch(head);
					m_ElementCount = m_ElementCount > 0 ? m_ElementCount - 1 : 0;
					if (temp) temp->Parent = nullptr;
					return temp;
				}

				BranchType* temp = GetInorderSuccessor(head->RightPart);
				head->Pair.Value1 = temp->Pair.Value1;
				head->Pair.Value2 = temp->Pair.Value2;
				head->RightPart = PErase(head->RightPart, temp->Pair.Value1);
				if (head->RightPart) head->RightPart->Parent = head;
			}

			head->Height = 1 + ME::Core::Algorithm::Max(GetHeight(head->LeftPart), GetHeight(head->RightPart));
			int32 balanceFactor = GetBalanceFactor(head);

			if (balanceFactor > 1 && GetBalanceFactor(head->LeftPart) >= 0)
				return RotateRight(head);
			if (balanceFactor < -1 && GetBalanceFactor(head->RightPart) <= 0)
				return RotateLeft(head);
			if (balanceFactor > 1 && GetBalanceFactor(head->LeftPart) < 0)
			{
				head->LeftPart = RotateLeft(head->LeftPart);
				if (head->LeftPart) head->LeftPart->Parent = head;
				return RotateRight(head);
			}
			if (balanceFactor < -1 && GetBalanceFactor(head->RightPart) > 0)
			{
				head->RightPart = RotateRight(head->RightPart);
				if (head->RightPart) head->RightPart->Parent = head;
				return RotateLeft(head);
			}

			return head;
		}

		void PClear(BranchType* head)
		{
			if (head == nullptr)
				return;
		    if (head->LeftPart != nullptr)
				PClear(head->LeftPart);
			if (head->RightPart != nullptr)
				PClear(head->RightPart);

			DeallocateBranch(head);
		}

	private:
		inline BranchType* GetInorderSuccessor(BranchType* branch) const
		{
			return branch == nullptr || branch->LeftPart == nullptr
		        ? branch
		        : GetInorderSuccessor(branch->LeftPart);
		}

		inline int32 GetBalanceFactor(BranchType* branch) const
		{
			return branch == nullptr ? 0
				: int32(GetHeight(branch->LeftPart)) - int32(GetHeight(branch->RightPart));
		}

		inline uint32 GetHeight(BranchType* branch) const
		{
			return branch == nullptr ? 0 : branch->Height;
		}

		BranchType* RotateLeft(BranchType* branch)
		{
			if (!branch) return branch;
			BranchType* newHead = branch->RightPart;
			if (!newHead) return branch;

			BranchType* middleBranch = newHead->LeftPart;

			BranchType* oldParent = branch->Parent;
			newHead->Parent = oldParent;
			if (oldParent)
			{
				if (oldParent->LeftPart == branch) oldParent->LeftPart = newHead;
				else if (oldParent->RightPart == branch) oldParent->RightPart = newHead;
			}

			newHead->LeftPart = branch;
			branch->Parent = newHead;

			branch->RightPart = middleBranch;
			if (middleBranch) middleBranch->Parent = branch;

			branch->Height = 1 + ME::Core::Algorithm::Max(GetHeight(branch->LeftPart), GetHeight(branch->RightPart));
			newHead->Height = 1 + ME::Core::Algorithm::Max(GetHeight(newHead->LeftPart), GetHeight(newHead->RightPart));

			return newHead;
		}

		BranchType* RotateRight(BranchType* branch)
		{
			if (!branch) return branch;
			BranchType* newHead = branch->LeftPart;
			if (!newHead) return branch;

			BranchType* middleBranch = newHead->RightPart;

			BranchType* oldParent = branch->Parent;
			newHead->Parent = oldParent;
			if (oldParent)
			{
				if (oldParent->LeftPart == branch) oldParent->LeftPart = newHead;
				else if (oldParent->RightPart == branch) oldParent->RightPart = newHead;
			}

			newHead->RightPart = branch;
			branch->Parent = newHead;

			branch->LeftPart = middleBranch;
			if (middleBranch) middleBranch->Parent = branch;

			branch->Height = 1 + ME::Core::Algorithm::Max(GetHeight(branch->LeftPart), GetHeight(branch->RightPart));
			newHead->Height = 1 + ME::Core::Algorithm::Max(GetHeight(newHead->LeftPart), GetHeight(newHead->RightPart));

			return newHead;
		}

		BranchType* AllocateBranch(const KeyType& key, const ValueType& value)
		{
			BranchType* branch = m_Allocator.Allocate(1);
			branch->Height = 1;
			branch->Parent = nullptr;
			branch->LeftPart = nullptr;
			branch->RightPart = nullptr;

			new (&branch->Pair.Value1) KeyType(key);
			new (&branch->Pair.Value2) ValueType(value);

		    return branch;
		}

		BranchType* AllocateBranch(KeyType&& key, ValueType&& value)
		{
			BranchType* branch = m_Allocator.Allocate(1);
			branch->Height = 1;
			branch->Parent = nullptr;
			branch->LeftPart = nullptr;
			branch->RightPart = nullptr;

			new (&branch->Pair.Value1) KeyType(std::move(key));
			new (&branch->Pair.Value2) ValueType(std::move(value));

			return branch;
		}

		void DeallocateBranch(BranchType* branch)
		{
			branch->Pair.Value1.~KeyType();
			branch->Pair.Value2.~ValueType();

            m_Allocator.Deallocate(branch, 1);
		}

		BranchType* SearchBranch(const KeyType& key)
		{
			BranchType* branch = m_Root;
            while (branch != nullptr && key != branch->Pair.Value1)
            {
				if (key < branch->Pair.Value1)
					branch = branch->LeftPart;
				else
					branch = branch->RightPart;
            }
			return branch;
		}

		BranchType* GetLeftmostBranch()
		{
			BranchType* branch = m_Root;
			if (!branch) return nullptr;
			while (branch->LeftPart != nullptr)
				branch = branch->LeftPart;
			return branch;
		}

		BranchType* GetRightmostBranch()
		{
			BranchType* branch = m_Root;
			if (!branch) return nullptr;
			while (branch->RightPart != nullptr)
				branch = branch->RightPart;
			return branch;
		}

		BranchType* CloneBranch(BranchType* other)
		{
			BranchType* branch = AllocateBranch(other->Pair.Value1, other->Pair.Value2);
			if (other->LeftPart != nullptr)
				branch->LeftPart = CloneBranch(other->LeftPart);
			if (other->RightPart != nullptr)
				branch->RightPart = CloneBranch(other->RightPart);
			return branch;
		}

	private:
		BranchType* m_Root;
		AllocatorType m_Allocator;

	private:
		SIZE_T m_ElementCount;

	};
}
