#pragma once

#include "Core.hpp"
#include "Core/Misc/Pair.hpp"
#include "Core/Memory/Hasher.hpp"
#include "Core/Memory/Allocators/Allocator.hpp"
#include "Core/Algorithm.hpp"

#define ME_MAP_ITER_CHECK(ret) if (this->m_Ptr == nullptr)			\
{																	\
    ME_ASSERT(false, "Can't iterate with ptr = nullptr!");	\
    return ret;														\
}

namespace ME::Core::Containers
{
	template <typename ValType>
	struct SetBranch
	{
		// Data
		ValType Value;
		uint32 Height;
		// Pointers
		SetBranch* Parent;
		SetBranch* LeftPart;
		SetBranch* RightPart;

		SetBranch* Next()
		{
			SetBranch* next = nullptr;
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

		SetBranch* Last()
		{
			SetBranch* last = nullptr;
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

	template<typename _Set>
	class SetIterator
	{
	public:
		using DataType = typename _Set::ValueType;
		using PtrType = _Set::SetBranchType*;
		using RefType = DataType&;

	public:
		SetIterator() : m_Ptr(nullptr) {}
		SetIterator(PtrType ptr) : m_Ptr(ptr) {}

	public:

		SetIterator operator-(const SIZE_T& index)
		{
			ME_MAP_ITER_CHECK(SetIterator(nullptr));
			PtrType temp = m_Ptr;
			for (SIZE_T i = 0; i < index; i++)
				temp = temp->Next();
			return SetIterator(temp);
		}

		SetIterator operator+(const SIZE_T& index)
		{
			ME_MAP_ITER_CHECK(SetIterator(nullptr));
			PtrType temp = m_Ptr;
			for (SIZE_T i = 0; i < index; i++)
				temp = temp->Last();
			return SetIterator(temp);
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

		SetIterator& operator++()
		{
			this->operator+=(1);
			return (*this);
		}

		SetIterator operator++(int)
		{
			SetIterator temp = (*this);
			this->operator+=(1);
			return temp;
		}

		SetIterator& operator--()
		{
			ME_MAP_ITER_CHECK(SetIterator(nullptr));
			this->operator-=(1);
			return (*this);
		}

		SetIterator operator--(int)
		{
			ME_MAP_ITER_CHECK(SetIterator(nullptr));
			SetIterator temp = (*this);
			this->operator-=(1);
			return temp;
		}

		bool operator!=(const SetIterator& it)
		{
			return (m_Ptr != it.m_Ptr);
		}

		bool operator==(const SetIterator& it)
		{
			return (m_Ptr == it.m_Ptr);
		}

		SetIterator& operator= (const SetIterator& it)
		{
			this->m_Ptr = it.m_Ptr;
			return *this;
		}

		RefType operator[] (SIZE_T index)
		{
			ME_MAP_ITER_CHECK(SetIterator(nullptr));
			return this->operator+(index).m_Ptr->Pair;
		}

		DataType* operator->()
		{
			return &m_Ptr->Value;
		}

		RefType operator*()
		{
			return m_Ptr->Value;
		}

	public:
		PtrType m_Ptr;

	};

	template <typename valType, class _allocator =
		Memory::Allocator<SetBranch<valType>>>
	class Set
	{
	public:
		using ValueType = valType;

		using SetBranchType = SetBranch<ValueType>;
		using AllocatorType = _allocator;

		using Iterator = SetIterator<Set>;

	public:
		constexpr Set() :
			m_Root(nullptr), m_Allocator(AllocatorType()), m_ElementCount(0) {}

		Set(const Set& other) noexcept
		{
			m_ElementCount = other.m_ElementCount;
			m_Allocator = other.m_Allocator;
			m_Root = CloneSetBranch(other.m_Root);
		}

		Set(Set&& other) noexcept
		{
			m_Allocator = other.m_Allocator;
			m_ElementCount = other.m_ElementCount;
			m_Root = other.m_Root;

			other.m_ElementCount = 0;
			other.m_Root = nullptr;
		}

		~Set()
		{
			m_ElementCount = 0;
			Clear();
		}

	public:
		inline void Insert(const ValueType& value)
		{
			m_Root = PInsert(m_Root, value);
			++m_ElementCount;
		}

		inline void Insert(ValueType&& value)
		{
			m_Root = PInsert(m_Root, std::move(value));
			++m_ElementCount;
		}

		inline void Erase(const ValueType& value)
		{
			m_Root = PErase(m_Root, value);
		}

		inline void Clear()
		{
			PClear(m_Root);
			m_Root = nullptr;
			m_ElementCount = 0;
		}

		inline Iterator Find(const ValueType& value)
		{
			return Iterator(SearchSetBranch(value));
		}

		ME_NODISCARD inline SIZE_T Size() const
		{
			return m_ElementCount;
		}

		ME_NODISCARD inline bool Empty() const
		{
			return m_Root == nullptr;
		}

		ME_NODISCARD inline const SetBranchType* Data() const
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
			return Iterator(GetLeftmostSetBranch());
		}

		inline Iterator End()
		{
			return Iterator(nullptr);
		}

		ME_NODISCARD inline ValueType& Front()
		{
			return GetLeftmostSetBranch()->Value;
		}

		ME_NODISCARD inline ValueType& Back()
		{
			return GetRightmostSetBranch()->Value;
		}

	private:
		SetBranchType* PInsert(SetBranchType* root, const ValueType& value)
		{
			if (root == nullptr)
				return AllocateSetBranch(value);
			else if (value < root->Value)
			{
				root->LeftPart = PInsert(root->LeftPart, value);
				if (root->LeftPart) root->LeftPart->Parent = root;
			}
			else
			{
				root->RightPart = PInsert(root->RightPart, value);
				if (root->RightPart) root->RightPart->Parent = root;
			}

			root->Height = 1 + ME::Core::Algorithm::Max(GetHeight(root->LeftPart), GetHeight(root->RightPart));
			int32 balanceFactor = GetBalanceFactor(root);

			if (balanceFactor > 1 && value < root->LeftPart->Value)
				return RotateRight(root);
			if (balanceFactor < -1 && value > root->RightPart->Value)
				return RotateLeft(root);
			if (balanceFactor > 1 && value > root->LeftPart->Value)
			{
				root->LeftPart = RotateLeft(root->LeftPart);
				if (root->LeftPart) root->LeftPart->Parent = root;
				return RotateRight(root);
			}
			if (balanceFactor < -1 && value < root->RightPart->Value)
			{
				root->RightPart = RotateRight(root->RightPart);
				if (root->RightPart) root->RightPart->Parent = root;
				return RotateLeft(root);
			}

			return root;
		}


		SetBranchType* PInsert(SetBranchType* root, ValueType&& value)
		{
			if (root == nullptr)
				return AllocateSetBranch(value);
			else if (value < root->Value)
			{
				root->LeftPart = PInsert(root->LeftPart, std::move(value));
				root->LeftPart->Parent = root;
			}
			else
			{
				root->RightPart = PInsert(root->RightPart, std::move(value));
				root->RightPart->Parent = root;
			}

			root->Height = 1 + ME::Core::Algorithm::Max(GetHeight(root->LeftPart), GetHeight(root->RightPart));

			int32 balanceFactor = GetBalanceFactor(root);

			if (balanceFactor > 1 && value < root->LeftPart->Value)
				return RotateRight(root);
			if (balanceFactor < -1 && value > root->RightPart->Value)
				return RotateLeft(root);
			if (balanceFactor > 1 && value > root->LeftPart->Value)
			{
				root->LeftPart = RotateLeft(root->LeftPart);
				return RotateRight(root);
			}
			if (balanceFactor < -1 && value < root->RightPart->Value)
			{
				root->RightPart = RotateRight(root->RightPart);
				return RotateLeft(root);
			}

			return root;
		}

		SetBranchType* PErase(SetBranchType* head, const ValueType& value)
		{
			if (head == nullptr)
				return head;

			if (value < head->Value)
			{
				head->LeftPart = PErase(head->LeftPart, value);
				if (head->LeftPart) head->LeftPart->Parent = head;
			}
			else if (value > head->Value)
			{
				head->RightPart = PErase(head->RightPart, value);
				if (head->RightPart) head->RightPart->Parent = head;
			}
			else
			{
				if (head->LeftPart == nullptr || head->RightPart == nullptr)
				{
					SetBranchType* temp = head->LeftPart ? head->LeftPart : head->RightPart;
					DeallocateSetBranch(head);
					m_ElementCount = m_ElementCount > 0 ? m_ElementCount - 1 : 0;
					if (temp) temp->Parent = nullptr;
					return temp;
				}

				SetBranchType* temp = GetInorderSuccessor(head->RightPart);
				head->Value = temp->Value;
				head->Pair.Value2 = temp->Pair.Value2;
				head->RightPart = PErase(head->RightPart, temp->Value);
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

		void PClear(SetBranchType* head)
		{
			if (head == nullptr)
				return;
			if (head->LeftPart != nullptr)
				PClear(head->LeftPart);
			if (head->RightPart != nullptr)
				PClear(head->RightPart);

			DeallocateSetBranch(head);
		}

	private:
		inline SetBranchType* GetInorderSuccessor(SetBranchType* branch) const
		{
			return branch == nullptr || branch->LeftPart == nullptr
				? branch
				: GetInorderSuccessor(branch->LeftPart);
		}

		inline int32 GetBalanceFactor(SetBranchType* branch) const
		{
			return branch == nullptr ? 0
				: int32(GetHeight(branch->LeftPart)) - int32(GetHeight(branch->RightPart));
		}

		inline uint32 GetHeight(SetBranchType* branch) const
		{
			return branch == nullptr ? 0 : branch->Height;
		}

		SetBranchType* RotateLeft(SetBranchType* branch)
		{
			if (!branch) return branch;
			SetBranchType* newHead = branch->RightPart;
			if (!newHead) return branch;

			SetBranchType* middleSetBranch = newHead->LeftPart;

			SetBranchType* oldParent = branch->Parent;
			newHead->Parent = oldParent;
			if (oldParent)
			{
				if (oldParent->LeftPart == branch) oldParent->LeftPart = newHead;
				else if (oldParent->RightPart == branch) oldParent->RightPart = newHead;
			}

			newHead->LeftPart = branch;
			branch->Parent = newHead;

			branch->RightPart = middleSetBranch;
			if (middleSetBranch) middleSetBranch->Parent = branch;

			branch->Height = 1 + ME::Core::Algorithm::Max(GetHeight(branch->LeftPart), GetHeight(branch->RightPart));
			newHead->Height = 1 + ME::Core::Algorithm::Max(GetHeight(newHead->LeftPart), GetHeight(newHead->RightPart));

			return newHead;
		}

		SetBranchType* RotateRight(SetBranchType* branch)
		{
			if (!branch) return branch;
			SetBranchType* newHead = branch->LeftPart;
			if (!newHead) return branch;

			SetBranchType* middleSetBranch = newHead->RightPart;

			SetBranchType* oldParent = branch->Parent;
			newHead->Parent = oldParent;
			if (oldParent)
			{
				if (oldParent->LeftPart == branch) oldParent->LeftPart = newHead;
				else if (oldParent->RightPart == branch) oldParent->RightPart = newHead;
			}

			newHead->RightPart = branch;
			branch->Parent = newHead;

			branch->LeftPart = middleSetBranch;
			if (middleSetBranch) middleSetBranch->Parent = branch;

			branch->Height = 1 + ME::Core::Algorithm::Max(GetHeight(branch->LeftPart), GetHeight(branch->RightPart));
			newHead->Height = 1 + ME::Core::Algorithm::Max(GetHeight(newHead->LeftPart), GetHeight(newHead->RightPart));

			return newHead;
		}

		SetBranchType* AllocateSetBranch(const ValueType& value)
		{
			SetBranchType* branch = m_Allocator.Allocate(1);
			branch->Height = 1;
			branch->Value = value;
			branch->Pair.Value2 = value;
			branch->Parent = nullptr;
			branch->LeftPart = nullptr;
			branch->RightPart = nullptr;
			return branch;
		}

		SetBranchType* AllocateSetBranch(ValueType&& value)
		{
			SetBranchType* branch = m_Allocator.Allocate(1);
			branch->Height = 1;
			branch->Value = std::move(value);
			branch->Pair.Value2 = std::move(value);
			branch->Parent = nullptr;
			branch->LeftPart = nullptr;
			branch->RightPart = nullptr;
			return branch;
		}

		void DeallocateSetBranch(SetBranchType* branch)
		{
			m_Allocator.Deallocate(branch, 1);
		}

		SetBranchType* SearchSetBranch(const ValueType& value)
		{
			SetBranchType* branch = m_Root;
			while (branch != nullptr && value != branch->Value)
			{
				if (value < branch->Value)
					branch = branch->LeftPart;
				else
					branch = branch->RightPart;
			}
			return branch;
		}

		SetBranchType* GetLeftmostSetBranch()
		{
			SetBranchType* branch = m_Root;
			if (!branch) return nullptr;
			while (branch->LeftPart != nullptr)
				branch = branch->LeftPart;
			return branch;
		}

		SetBranchType* GetRightmostSetBranch()
		{
			SetBranchType* branch = m_Root;
			if (!branch) return nullptr;
			while (branch->RightPart != nullptr)
				branch = branch->RightPart;
			return branch;
		}

		SetBranchType* CloneSetBranch(SetBranchType* other)
		{
			SetBranchType* branch = AllocateSetBranch(other->Value, other->Pair.Value2);
			if (other->LeftPart != nullptr)
				branch->LeftPart = CloneSetBranch(other->LeftPart);
			if (other->RightPart != nullptr)
				branch->RightPart = CloneSetBranch(other->RightPart);
			return branch;
		}

	private:
		SetBranchType* m_Root;
		AllocatorType m_Allocator;

	private:
		SIZE_T m_ElementCount;

	};
}
