#pragma once
#include "Core/Core.h"
#include "Core/Utils/Logging/Logger.h"
#include "Core/Memory/PawnMemory.h"
#include "Core/Misc/CoreAssertion.h"

#define RESIZE_MULTIPLYER 2

template<typename Array>
class PAWN_API ArrayIterator
{
public:
	using DataType = typename Array::DataType;
	using PtrType = DataType*;
	using RefType = DataType&;

public:
	ArrayIterator(PtrType ptr) : m_Ptr(ptr) {};
	//Array_Iterator(const DataType*& ptr) = delete;
	//Array_Iterator(DataType*&& ptr) = delete;

	~ArrayIterator()
	{
		m_Ptr = nullptr;
	}

public:

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

	RefType operator[] (SIZE_T index)
	{
		return *(m_Ptr + index);
	}

	PtrType operator->()
	{
		return m_Ptr;
	}

	bool operator!=(const ArrayIterator& it)
	{
		return (m_Ptr != it.m_Ptr);
	}

	bool operator==(const ArrayIterator& it)
	{
		return (m_Ptr == it.m_Ptr);
	}

	RefType operator*()
	{
		return *m_Ptr;
	}

private:
	PtrType m_Ptr;

};

template<typename T, SIZE_T initSize = 20, class allocator = Allocator<T>>
class PAWN_API Array
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

		Allocate(other.size() > initSize ? other.size() * RESIZE_MULTIPLYER : initSize);

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
			::operator delete(m_Data, m_Capacity * sizeof(DataType));
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
		PE_CORE_ASSERT(index >= m_Size, "Index in array is out of range!");
		auto& returnVal = m_Data[index];
		return returnVal;
	}

	const ReturnType& operator[](const SIZE_T index) const noexcept
	{
		PE_CORE_ASSERT(index >= m_Size, "Index in array is out of range!");
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
			Allocate(m_Capacity * RESIZE_MULTIPLYER);
	}

	constexpr void CheckAndDeallocate()
	{
		SIZE_T newSize = m_Capacity * (1.f / (float32)RESIZE_MULTIPLYER);
		if (m_Size <= newSize)
			Allocate(newSize);
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

	void PPopBack()
	{
		if (m_Size > 0)
		{
			m_Size--;
			m_Data[m_Size].~DataType();
		}
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
	SIZE_T m_Capacity ;
};