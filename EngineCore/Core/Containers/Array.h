#pragma once
#include "Core.h"
#include "Core/Utils/Logging/Logger.h"
#include "Core/Memory/PawnMemory.h"
#include "Core/Misc/Assertion.h"

#define ARR_RESIZE_MULTIPLYER 2

namespace Pawn
{
	template<typename _Array>
	class ArrayIterator
	{
	public:
		//using IteratorB = Memory::BaseIterator<_Array>;
		using DataType = typename _Array::DataType;
		using PtrType = DataType*;
		using RefType = DataType&;


	public:
		ArrayIterator() : m_Ptr(nullptr) {}
		ArrayIterator(PtrType ptr) : m_Ptr(ptr) {}

	public:

		ArrayIterator operator-(const SIZE_T& index)
		{
			ArrayIterator it = *this;
			it -= index;
			return (*this);
		}

		ArrayIterator operator+(const SIZE_T& index)
		{
			ArrayIterator it = *this;
			it += index;
			return it;
		}

		void operator-=(const SIZE_T& index)
		{
			this->m_Ptr -= index;
		}

		void operator+=(const SIZE_T& index)
		{
			this->m_Ptr += index;
		}

		ArrayIterator& operator++()
		{
			m_Ptr++;
			return (*this);
		}

		ArrayIterator operator++(int)
		{
			ArrayIterator temp = (*this);
			m_Ptr++;
			return temp;
		}

		ArrayIterator& operator--()
		{
			m_Ptr--;
			return (*this);
		}

		ArrayIterator operator--(int)
		{
			ArrayIterator temp = (*this);
			m_Ptr--;
			return temp;
		}

		bool operator!=(const ArrayIterator& it)
		{
			return (m_Ptr != it.m_Ptr);
		}

		bool operator==(const ArrayIterator& it)
		{
			return (m_Ptr == it.m_Ptr);
		}

		ArrayIterator& operator= (const ArrayIterator& it)
		{
			this->m_Ptr = it.m_Ptr;
			return *this;
		}

		RefType operator[] (SIZE_T index)
		{
			return *(m_Ptr + index);
		}

		PtrType operator->()
		{
			return m_Ptr;
		}

		RefType operator*()
		{
			return *m_Ptr;
		}

	public:
		PtrType m_Ptr;

	};

	template<typename T, SIZE_T initSize = 20, class allocator = Memory::Allocator<T>>
	class CORE_API Array
	{
	public:
		using DataType = T;
		using ReturnType = DataType;
		using Ptr = T*;
		using Iterator = ArrayIterator<Array>;
		using AllocatorType = allocator;

	public:
		Array()
			: m_Data(nullptr), m_Size(0), m_Capacity(initSize), m_Allocator(AllocatorType())
		{
			Allocate(m_Capacity);
		}

		Array(const Array& other)
		{
			m_Allocator = other.m_Allocator;
			m_Size = other.m_Size;
			m_Capacity = other.m_Capacity;
			m_Data = nullptr;

			Allocate(m_Capacity);

			for (SIZE_T i = 0; i < m_Size; i++)
				m_Data[i] = other.m_Data[i];
		}

		Array(const std::initializer_list<DataType>& other)
			: m_Data(nullptr), m_Allocator(AllocatorType())
		{
			m_Size = other.size();

			Allocate(other.size() > initSize ? other.size() * ARR_RESIZE_MULTIPLYER : initSize);

			for (SIZE_T i = 0; i < other.size(); i++)
				new(&m_Data[i]) DataType(std::move(other.begin()[i]));
		}

		Array(Array&& other) noexcept
		{
			m_Allocator = other.m_Allocator;
			m_Size = other.m_Size;
			m_Capacity = other.m_Capacity;

			other.m_Size = 0;
			other.m_Capacity = 0;

			m_Data = other.m_Data;
			other.m_Data = nullptr;
		}

		~Array()
		{
			Clear();

			if (m_Data)
			{
				m_Allocator.Deallocate(m_Data, m_Capacity * sizeof(DataType));
				m_Data = nullptr;
			}
		}

	public:

		inline SIZE_T GetSize() const
		{
			return m_Size;
		}

		inline SIZE_T GetCapacity() const
		{
			return m_Capacity;
		}

		inline Iterator begin() const
		{
			return Begin();
		}

		inline Iterator end() const
		{
			return End();
		}

		inline Iterator Begin() const
		{
			return Iterator(m_Data);
		}

		inline Iterator End() const
		{
			return Iterator(m_Data + m_Size);
		}

	public:

		ReturnType& operator[](const SIZE_T index) noexcept
		{
			PE_CORE_ASSERT(index < m_Size, "Index in array is out of range!");
			auto& returnVal = m_Data[index];
			return returnVal;
		}

		const ReturnType& operator[](const SIZE_T index) const noexcept
		{
			PE_CORE_ASSERT(index < m_Size, "Index in array is out of range!");
			auto& returnVal = m_Data[index];
			return returnVal;
		}

	public:
		void Clear()
		{
			for (SIZE_T i = 0; i < m_Size; i++)
				m_Data[i].~DataType();
			m_Size = 0;
		}

		bool Resize(SIZE_T size)
		{
			return PResize(size);
		}

	public:

		template <class... val>
		Iterator Emplace(ArrayIterator<Array> it, val&&... args)
		{
			return PEmplace(it, std::forward<val>(args)...);
		}

		void PushBack(const DataType& value)
		{
			PPushBack(value);
		}

		void PushBack(DataType&& value) noexcept
		{
			PPushBackRVal(std::move(value));
		}

		template <class... val>
		ReturnType& EmplaceBack(val&&... args)
		{
			return PEmplaceBack(std::forward<val>(args)...);
		}

		Iterator Erase(const Iterator it)
		{
			return PErase(it);
		}

		void PopBack()
		{
			PPopBack();
		}

	private:
		void Allocate(SIZE_T newCapacity)
		{
			DataType* newBlock = m_Allocator.Allocate(newCapacity * sizeof(DataType));

			if (newCapacity < m_Size)
				m_Size = newCapacity;

			if (m_Data && m_Capacity > 0)
			{
				for (SIZE_T i = 0; i < m_Size; i++)
					new(&newBlock[i]) DataType(std::move(m_Data[i]));

				for (SIZE_T i = 0; i < m_Size; i++)
					m_Data[i].~DataType();

				m_Allocator.Deallocate(m_Data, m_Capacity * sizeof(DataType));
			}

			m_Data = newBlock;
			m_Capacity = newCapacity;
		}

		inline void CheckAndAllocate()
		{
			if (m_Size >= m_Capacity)
				Allocate(m_Capacity * ARR_RESIZE_MULTIPLYER);
		}

	private:
		void PPushBack(const DataType& value)
		{
			CheckAndAllocate();
			m_Data[m_Size] = value;
			m_Size++;
		}

		void PPushBackRVal(DataType&& value)
		{
			CheckAndAllocate();

			m_Data[m_Size] = std::move(value);
			m_Size++;
		}

		template <class... val>
		ReturnType& PEmplaceBack(val&&... args)
		{
			CheckAndAllocate();

			Ptr lastLoc = &m_Data[m_Size];

			m_Allocator.Construct(lastLoc, std::forward<val>(args)...);
			m_Size++;

			DataType& result = *lastLoc;

			return result;
		}

		template <class... val>
		Iterator PEmplace(ArrayIterator<Array> it, val&&... args)
		{
			Ptr itPtr = it.m_Ptr;
			Ptr lastLoc = &m_Data[m_Size];
		
			CheckAndAllocate();

			if (itPtr == lastLoc)
			{
				m_Allocator.Construct(lastLoc, std::forward<val>(args)...);
				m_Size++;
			}
			else
			{
				m_Allocator.Construct(lastLoc, std::forward<val>(args)...);
				for (Ptr pos = lastLoc - 1; pos > itPtr; --pos)
					*pos = std::move(*(pos - 1));

				itPtr->~DataType();
				m_Allocator.Construct(itPtr, std::forward<val>(args)...);
				m_Size++;
			}

			return Iterator(itPtr);
		}

		void PPopBack()
		{
			if (m_Size > 0)
			{
				m_Size--;
				m_Data[m_Size].~DataType();
			}
		}

		Iterator PErase(const Iterator it)
		{
			Ptr wherePtr = it.m_Ptr;

			for (Ptr pos = wherePtr; pos < m_Data + m_Size - 1; ++pos)
				*pos = std::move(*(pos + 1));

			m_Allocator.Destroy(m_Data + m_Size - 1);

			--m_Size;

			return Iterator(wherePtr);
		}

		bool PResize(SIZE_T size)
		{
			if (size <= m_Capacity)
				return false;

			Allocate(size);

			return true;
		}

	private:
		AllocatorType m_Allocator;

	private:
		DataType* m_Data;

		SIZE_T m_Size;
		SIZE_T m_Capacity;
	};

}