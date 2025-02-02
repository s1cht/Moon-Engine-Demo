#pragma once
#include "Core/Core.h"
#include "Core/Utils/Logging/Logger.h"

template<typename Array>
class PAWN_API Array_Iterator
{
public:
	using DataType = Array::DataType;

private:
	DataType* m_Ptr;

};

template<typename T, SIZE_T stackSize = 20>
class PAWN_API Array
{
public:
	using DataType = T;
	using Iterator = Array_Iterator<Array>;

public:
	Array()
		: m_HeapData(nullptr), m_Size(0), m_Capacity(stackSize), m_UsesStackMemory(true) {}

	Array(const Array& other)
	{
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
		: m_HeapData(nullptr)
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

		ChangeMemoryType(m_Capacity);

		for (SIZE_T i = 0; i < other.size(); i++)
		{
			m_HeapData[i] = other.begin()[i];
		}
	}
	
	Array(Array&& other) noexcept
	{
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


private:
	void Allocate(SIZE_T newCapacity)
	{
		DataType* newBlock = (DataType*)::operator new(newCapacity * sizeof(DataType));

		if (newCapacity < m_Size)
		{
			m_Size = newCapacity;
		}

		for (SIZE_T i = 0; i < m_Size; i++)
			new(&newBlock[i]) DataType(std::move(m_HeapData[i]));

		for (SIZE_T i = 0; i < m_Size; i++)
			m_HeapData[i].~DataType();

		::operator delete(m_HeapData, m_Capacity * sizeof(DataType));
		m_HeapData = newBlock;
		m_Capacity = newCapacity;
	}

	void ChangeMemoryType(SIZE_T size)
	{
		m_HeapData = (DataType*)::operator new(size * sizeof(DataType));
	
		CopyValuesFromStack();

		m_UsesStackMemory = false;
	}

	void CopyValuesFromStack()
	{
		for (SIZE_T i = 0; i < stackSize; i++)
			new(&m_HeapData[i]) DataType(std::move(m_StackData[i]));
	}

private:
	// Stack data buffer
	DataType m_StackData[stackSize];
	// If stack buffer exceeds stack size, stack freed, and array becomes "vector"
	DataType* m_HeapData;

	SIZE_T m_Size;
	SIZE_T m_Capacity;

	bool m_UsesStackMemory;
};