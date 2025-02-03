#pragma once
#include "Core/Core.h"
#include "Core/Utils/Logging/Logger.h"
#include "Core/Memory/PawnMemory.h"

#include <vector>

template<typename Array>
class PAWN_API Array_Iterator
{
public:
	using DataType = Array::DataType;

private:
	DataType* m_Ptr;

};

template<typename T, SIZE_T stackSize = 20, class allocator = Allocator<T>>
class PAWN_API Array
{
public:
	using DataType = T;
	using Ptr = T*;
	using Iterator = Array_Iterator<Array>;
	using AllocatorType = allocator;

public:
	Array()
		: m_HeapData(nullptr), m_Size(0), m_Capacity(stackSize), m_UsesStackMemory(true), m_Allocator(AllocatorType()) {}

	Array(const Array& other)
	{
		m_Allocator = other.m_Allocator;
		m_HeapData = nullptr;
		m_Size = other.m_Size;
		m_Capacity = other.m_Capacity;
		m_UsesStackMemory = other.m_UsesStackMemory;

		if (m_UsesStackMemory)
		{
			for (SIZE_T i = 0; i < m_Size; i++)
				m_StackData[i] = other.m_StackData[i];
			return;
		}

		ChangeMemoryType(m_Capacity);

		for (SIZE_T i = 0; i < other.size(); i++)
		{
			m_HeapData[i] = other.m_HeapData()[i];
		}
	}
	
	Array(const std::initializer_list<DataType>& other)
		: m_HeapData(nullptr), m_Allocator(AllocatorType())
	{
		m_Size = other.size();
		m_Capacity = stackSize;
		m_UsesStackMemory = true;

		if (m_Size <= m_Capacity)
		{
			for (SIZE_T i = 0; i < other.size(); i++)
			{
				m_StackData[i] = other.begin()[i];
			}
			return;
		}

		while (m_Size > m_Capacity)
			m_Capacity *= 1.5;

		ChangeMemoryType(m_Capacity, false);

		for (SIZE_T i = 0; i < other.size(); i++)
			new(&m_HeapData[i]) DataType(std::move(other.begin()[i]));
	}
	
	Array(Array&& other) noexcept
	{
		m_Allocator = other.m_Allocator;
		m_Size = other.m_Size;
		m_Capacity = other.m_Capacity;
		m_UsesStackMemory = other.m_UsesStackMemory;

		other.m_Size = 0;
		other.m_Capacity = 0;
		other.m_UsesStackMemory = false;

		if (m_UsesStackMemory)
		{
			for (SIZE_T i = 0; i < m_Size; i++)
				m_StackData[i] = std::move(other.m_StackData[i]);
		}
		else
		{
			m_HeapData = other.m_HeapData;
			other.m_HeapData = nullptr;
		}
	}

	~Array()
	{
		Clear();

		if (!m_UsesStackMemory)
			::operator delete(m_HeapData, m_Capacity * sizeof(DataType));
	}

public:
	void Clear()
	{
		if (m_UsesStackMemory)
		{
			for (SIZE_T i = 0; i < m_Size; i++)
				m_StackData[i].~DataType();
		}
		else
		{
			for (SIZE_T i = 0; i < m_Size; i++)
				m_HeapData[i].~DataType();
		}
		m_Size = 0;
	}

public:
	void PushBack(const DataType& value)
	{
		if (!m_UsesStackMemory)
		{
			PPushBackHeap();
			return;
		}
		PPushBackStack(value);
	}

	void PushBack(DataType&& value) noexcept
	{
		if (!m_UsesStackMemory)
		{
			PPushBackHeap(value);
			return;
		}
		PPushBackStack(value);
	}

	template <class... val>
	DataType& EmplaceBack(val&&... args)
	{
		if (!m_UsesStackMemory)
			return PEmplaceBackHeap(std::forward<DataType>(args)...);
		else
			return PEmplaceBackStack(std::forward<DataType>(args)...);
	}

private:
	void Allocate(SIZE_T newCapacity)
	{
		DataType* newBlock = m_Allocator.Allocate(newCapacity * sizeof(DataType));

		if (newCapacity < m_Size)
			m_Size = newCapacity;

		for (SIZE_T i = 0; i < m_Size; i++)
			new(&newBlock[i]) DataType(std::move(m_HeapData[i]));

		for (SIZE_T i = 0; i < m_Size; i++)
			m_HeapData[i].~DataType();

		m_Allocator.Deallocate(m_HeapData, m_Capacity * sizeof(DataType));

		m_HeapData = newBlock;
		m_Capacity = newCapacity;
	}

	void ChangeMemoryType(SIZE_T newCapacity, bool copyValues = true)
	{
		m_HeapData = m_Allocator.Allocate(newCapacity * sizeof(DataType));
	
		if (copyValues)
			CopyValuesFromStack();

		m_UsesStackMemory = false;
	}

	void CopyValuesFromStack()
	{
		for (SIZE_T i = 0; i < stackSize; i++)
			new(&m_HeapData[i]) DataType(std::move(m_StackData[i]));
	}

	inline void CheckAndAllocate()
	{
		if (m_Size >= m_Capacity)
			Allocate(m_Capacity * 1.5);
	}

private:
	void PPushBackHeap(const DataType& value)
	{
		CheckAndAllocate();

		m_HeapData[m_Size] = value;
		m_Size++;
	}

	void PPushBackHeap(DataType&& value)
	{
		CheckAndAllocate();

		m_HeapData[m_Size] = std::move(value);
		value.~DataType();
		m_Size++;
	}

	void PPushBackStack(const DataType& value)
	{
		if (m_Size >= stackSize)
		{
			ChangeMemoryType(m_Capacity * 1.5);
			PPushBackHeap(value);
		}

		m_StackData[m_Size] = value;
		m_Size++;
	}

	void PPushBackStack(DataType&& value)
	{
		if (m_Size >= stackSize)
		{
			ChangeMemoryType(m_Capacity * 1.5);
			PPushBackHeap(value);
		}

		m_StackData[m_Size] = std::move(value);
		value.~DataType();
		m_Size++;
	}

private:
	template <class... val>
	DataType& PEmplaceBackStack(val&&... args)
	{
		if (m_Size >= stackSize)
		{
			ChangeMemoryType(m_Capacity * 1.5);
			return PEmplaceBackHeap(std::forward<val>(args)...);
		}

		Ptr lastLoc = &m_StackData[m_Size];

		m_Allocator.Construct(lastLoc, std::forward<val>(args)...);
		m_Size++;

		DataType& result = *lastLoc;

		return result;
	}

	template <class... val>
	DataType& PEmplaceBackHeap(val&&... args)
	{
		CheckAndAllocate();

		Ptr lastLoc = &m_HeapData[m_Size];

		m_Allocator.Construct(lastLoc, std::forward<val>(args)...);
		m_Size++;
		
		DataType& result = *lastLoc;

		return result;
	}

private:
	AllocatorType m_Allocator;

private:
	// Stack data buffer
	DataType m_StackData[stackSize];
	// If stack buffer exceeds stack size, stack freed, and array becomes "vector"
	DataType* m_HeapData;

	SIZE_T m_Size;
	SIZE_T m_Capacity;

	bool m_UsesStackMemory;
};