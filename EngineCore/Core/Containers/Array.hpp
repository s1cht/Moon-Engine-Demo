#pragma once
#include "Core.hpp"
#include "Core/Algorithm.hpp"
#include "Core/Memory/Allocators/Allocator.hpp"
#include "Core/Utility/Logging/Logger.hpp"

#include <vector>

constexpr uint8 ARR_RESIZE_MULTIPLYER = 2;

namespace ME::Core
{
	template<typename _Array>
	class ArrayConstIterator;

	template<typename _Array>
	class ArrayIterator
	{
	public:
		using DataType = typename _Array::DataType;
		using PtrType = DataType*;
		using RefType = DataType&;

	public:
		ArrayIterator() : m_Ptr(nullptr) {}
		ArrayIterator(PtrType ptr) : m_Ptr(ptr) {}

	public:
		explicit operator ArrayConstIterator<_Array>() const
		{
			return ArrayConstIterator<_Array>(m_Ptr);
		}

		ArrayIterator operator-(const SIZE_T& index)
		{
			ArrayIterator it = *this;
			it -= index;
			return it;
		}

		SIZE_T operator-(const ArrayIterator& index) const { return m_Ptr - index.m_Ptr; }
		SIZE_T operator-(const ArrayConstIterator<_Array>& index) const { return m_Ptr - index.m_Ptr; }
		SIZE_T operator-(PtrType ptr) const { return m_Ptr - ptr; }

		ArrayIterator operator+(const SIZE_T& index)
		{
			ArrayIterator it = *this;
			it += index;
			return it;
		}

		void operator-=(const SIZE_T& index) { this->m_Ptr -= index; }
		void operator+=(const SIZE_T& index) { this->m_Ptr += index; }

		ArrayIterator& operator--() { --m_Ptr; return (*this); }
		ArrayIterator operator--(int) { ArrayIterator temp = (*this); --m_Ptr; return temp; }
		ArrayIterator& operator++() { ++m_Ptr; return (*this); }
		ArrayIterator operator++(int) { ArrayIterator temp = (*this); ++m_Ptr; return temp; }

		bool operator!=(const ArrayIterator& it) const { return (m_Ptr != it.m_Ptr); }
		bool operator==(const ArrayIterator& it) const { return (m_Ptr == it.m_Ptr); }

		ArrayIterator& operator=(const ArrayIterator& it) { this->m_Ptr = it.m_Ptr; return *this; }

		RefType operator[] (SIZE_T index) { return *(m_Ptr + index); }
		PtrType operator->() { return m_Ptr; }
		RefType operator*() { return *m_Ptr; }

		inline PtrType Ptr() { return m_Ptr; }

	public:
		PtrType m_Ptr;
		friend class ArrayConstIterator<_Array>;
	};

	template <typename _Array>
	class ArrayConstIterator
	{
	public:
		using DataType = typename _Array::DataType;
		using PtrType = DataType*;
		using RefType = DataType&;
		using ConstRefType = const DataType&;

	public:
		ArrayConstIterator() : m_Ptr(nullptr) {}
		ArrayConstIterator(PtrType ptr) : m_Ptr(ptr) {}
		ArrayConstIterator(const ArrayIterator<_Array>& it) : m_Ptr(it.m_Ptr) {}

		ArrayConstIterator operator-(const SIZE_T& index)
		{
			ArrayConstIterator it = *this;
			it -= index;
			return it;
		}

		SIZE_T operator-(const ArrayIterator<_Array>& index) const { return m_Ptr - index.m_Ptr; }
		SIZE_T operator-(const ArrayConstIterator& index) const { return m_Ptr - index.m_Ptr; }
		SIZE_T operator-(PtrType ptr) const { return m_Ptr - ptr; }

		ArrayConstIterator operator+(const SIZE_T& index)
		{
			ArrayConstIterator it = *this;
			it += index;
			return it;
		}

		void operator-=(const SIZE_T& index) { this->m_Ptr -= index; }
		void operator+=(const SIZE_T& index) { this->m_Ptr += index; }

		ArrayConstIterator& operator--() { --m_Ptr; return (*this); }
		ArrayConstIterator operator--(int) { ArrayConstIterator temp = (*this); --m_Ptr; return temp; }
		ArrayConstIterator& operator++() { ++m_Ptr; return (*this); }
		ArrayConstIterator operator++(int) { ArrayConstIterator temp = (*this); ++m_Ptr; return temp; }

		inline bool operator!=(const ArrayConstIterator& it) const { return (m_Ptr != it.m_Ptr); }
		inline bool operator==(const ArrayConstIterator& it) const { return (m_Ptr == it.m_Ptr); }

		ArrayConstIterator& operator= (const ArrayConstIterator& it) { this->m_Ptr = it.m_Ptr; return *this; }

		inline ConstRefType operator[] (SIZE_T index) const { return *(m_Ptr + index); }
		inline const PtrType operator->() const { return m_Ptr; }
		inline ConstRefType operator*() const { return *m_Ptr; }

		inline PtrType Ptr() { return m_Ptr; }

	private:
		PtrType m_Ptr;
		friend class ArrayIterator<_Array>;
	};

	template<class T, class allocator = Memory::Allocator<T>>
	class Array
	{
	public:
		using DataType = T;

	public:
		using ReturnType = DataType;
		using Ptr = T*;

	    using AllocatorType = allocator;

	    using Iterator = ArrayIterator<Array>;
	    using ConstIterator = ArrayConstIterator<Array>;

	public:
		Array()
			: m_Allocator(AllocatorType()), m_Data(nullptr), m_Size(0), m_Capacity(20)
		{
			Allocate(m_Capacity);
		}

		Array(SIZE_T size)
			: m_Allocator(AllocatorType()), m_Data(nullptr), m_Size(size), m_Capacity(size)
		{
			Allocate(m_Capacity);
			m_Size = size;
			for (SIZE_T i = 0; i < m_Size; i++)
				m_Allocator.Construct(&m_Data[i]);
		}

		Array(const Array& other)
		{
			if (this != &other)
			{
				m_Allocator = other.m_Allocator;
				m_Capacity = other.m_Capacity;
				m_Size = 0;
				m_Data = nullptr;

				Allocate(m_Capacity);

				m_Size = other.m_Size;
				for (SIZE_T i = 0; i < m_Size; i++)
					m_Allocator.Construct(&m_Data[i], other.m_Data[i]);
			}
		}

		Array(const std::initializer_list<DataType>& other)
			: m_Allocator(AllocatorType()), m_Data(nullptr)
		{
			m_Data = nullptr;
			m_Size = 0;

			Allocate(other.size() > 20 ? other.size() * ARR_RESIZE_MULTIPLYER : 20);

			m_Size = other.size();
			for (SIZE_T i = 0; i < other.size(); i++)
				m_Allocator.Construct(&m_Data[i], other.begin()[i]);
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
				m_Allocator.Deallocate(m_Data, m_Capacity);
				m_Data = nullptr;
			}
		}

	public:
		ME_NODISCARD inline SIZE_T Size() const
		{
			return m_Size;
		}

		ME_NODISCARD inline SIZE_T Capacity() const
		{
			return m_Capacity;
		}

		ME_NODISCARD inline DataType* Data()
		{
			return m_Data;
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
			return Iterator(m_Data);
		}

		inline Iterator End()
		{
			return Iterator(m_Data + m_Size);
		}

		inline ConstIterator begin() const
		{
			return CBegin();
		}

		inline ConstIterator end() const
		{
			return CEnd();
		}

		inline ConstIterator Begin() const
		{
			return CBegin();
		}

		inline ConstIterator End() const
		{
			return CEnd();
		}

		inline ConstIterator cbegin() const
		{
			return CBegin();
		}

		inline ConstIterator cend() const
		{
			return CEnd();
		}

		inline ConstIterator CBegin() const
		{
			return ConstIterator(m_Data);
		}

		inline ConstIterator CEnd() const
		{
			return ConstIterator(m_Data + m_Size);
		}

		ME_NODISCARD inline DataType& Front() 
		{
			return *(m_Data);
		}

		ME_NODISCARD inline DataType& Back() 
		{
			return *(m_Data + m_Size - 1);
		}

	public:
		Array& operator=(const Array& other)
		{
			if (this != &other)
			{
				Clear();
				if (m_Data)
				{
					m_Allocator.Deallocate(m_Data, m_Capacity);
					m_Data = nullptr;
				}
				m_Allocator = other.m_Allocator;

				Allocate(other.m_Capacity);

				m_Size = other.m_Size;
				for (SIZE_T i = 0; i < m_Size; i++)
					m_Allocator.Construct(&m_Data[i], other.m_Data[i]);
			}
			return *this;
		}

		Array& operator=(Array&& other) noexcept
        {
			m_Allocator = other.m_Allocator;
			m_Size = other.m_Size;
			m_Capacity = other.m_Capacity;

			other.m_Size = 0;
			other.m_Capacity = 0;

			m_Data = other.m_Data;
			other.m_Data = nullptr;
			return *this;
		}

		ReturnType& operator[](const SIZE_T index) noexcept
		{
			ME_CORE_ASSERT(index < m_Capacity, "Index in array is out of range!");
			return m_Data[index];
		}

		const ReturnType& operator[](const SIZE_T index) const noexcept
		{
			ME_CORE_ASSERT(index < m_Capacity, "Index in array is out of range!");
			return m_Data[index];
		}

	public:
		void Append(const Array& other)
		{
			PAppend(other);
		}

		void Append(Array&& other)
		{
			PAppend(std::move(other));
		}

		void Clear()
		{
			if (m_Size == 0)
				return;

			for (SIZE_T i = 0; i < m_Size; i++)
				m_Allocator.Destroy(&m_Data[i]);
			m_Size = 0;
		}

		bool Reserve(SIZE_T size)
		{
			return PReserve(size);
		}

		void Resize(SIZE_T size)
		{
			if (size <= m_Capacity)
				m_Size = size;
		}

		void Shrink()
		{
			PShrink();
		}

	public:
		ME_NODISCARD inline bool Empty()
		{
			return m_Size <= 0;
		}

	public:
		template <class... val>
		Iterator Emplace(ConstIterator it, val&&... args)
		{
			return PEmplace(it, std::forward<val>(args)...);
		}

		Iterator Insert(ConstIterator position, const DataType& value)
		{
			return PEmplace(position, value);
		}

		Iterator Insert(ConstIterator position, DataType&& value)
		{
			return PEmplace(position, std::move(value));
		}

		Iterator Insert(ConstIterator position, const SIZE_T count, const DataType& value)
		{
			return PInsert(position, count, value);
		}

		Iterator Insert(ConstIterator position, const std::initializer_list<DataType>& list)
		{
			return PInsert(position, list);
		}

		template <typename InputIt>
		Iterator Insert(ConstIterator position, InputIt first, InputIt last)
		{
			return PInsert<InputIt>(position, first, last);
		}

		void PushBack(const DataType& value)
		{
			PPushBack(value);
		}

		void PushBack(DataType&& value) noexcept
		{
			PPushBack(std::move(value));
		}

		template <class... val>
		ReturnType& EmplaceBack(val&&... args) noexcept
		{
			return PEmplaceBack(std::forward<val>(args)...);
		}

		Iterator Erase(Iterator it)
		{
			return PErase(it);
		}

		Iterator Erase(ConstIterator it)
		{
			return PErase(it);
		}

		Iterator Erase(Iterator first, Iterator last)
		{
			return PErase(first, last);
		}

		Iterator Erase(ConstIterator first, ConstIterator last)
		{
			return PErase(first, last);
		}

		Iterator EraseAt(SIZE_T index)
		{
			return PErase(Begin() + index);
		}

		void PopBack()
		{
			PPopBack();
		}

	private:
		void Allocate(SIZE_T newCapacity)
		{
		    DataType* newBlock = m_Allocator.Allocate(newCapacity);

			SIZE_T elementsToMove = (m_Data && m_Capacity > 0) ? ME::Core::Algorithm::Min(m_Size, newCapacity) : 0;

			if (m_Data && m_Capacity > 0)
			{
				if constexpr (std::is_trivially_copyable_v<DataType>)
				{
					memcpy(newBlock, m_Data, elementsToMove * sizeof(DataType));
				}
				else
				{
					for (SIZE_T i = 0; i < elementsToMove; i++)
					{
						new (&newBlock[i]) DataType(std::move(m_Data[i]));
						m_Data[i].~DataType();
					}
				}

				m_Allocator.Deallocate(m_Data, m_Capacity);
			}

			m_Data = newBlock;
			m_Capacity = newCapacity;
			m_Size = elementsToMove;
		}

		inline void CheckAndAllocate()
		{
			if (m_Size >= m_Capacity)
				Allocate(m_Capacity <= 0 ? 20 : m_Capacity * ARR_RESIZE_MULTIPLYER);
		}

	private:
		void PPushBack(const DataType& value)
		{
			CheckAndAllocate();
			m_Allocator.Construct(&m_Data[m_Size], value);
			m_Size++;
		}

		void PPushBack(DataType&& value)
		{
			CheckAndAllocate();
			m_Allocator.Construct(&m_Data[m_Size], std::move(value));
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
		Iterator PEmplace(ConstIterator it, val&&... args)
		{
			SIZE_T offset = it - m_Data;

			Ptr itPtr = m_Data + offset;
			Ptr lastLoc = &m_Data[m_Size];

			CheckAndAllocate();

			if (itPtr == lastLoc)
			{
				m_Allocator.Construct(lastLoc, std::forward<val>(args)...);
				m_Size++;
			}
			else
			{
				for (Ptr pos = lastLoc; pos > itPtr; --pos)
				{
					m_Allocator.Construct(pos, std::move(*(pos - 1)));
					m_Allocator.Destroy(pos - 1);
				}

				m_Allocator.Destroy(itPtr);
				m_Allocator.Construct(itPtr, std::forward<val>(args)...);
				m_Size++;
			}

			return Iterator(itPtr);
		}

		void PInsertShift(SIZE_T offset, SIZE_T count)
		{
			if (count == 0) return;

			SIZE_T new_size = m_Size + count;
			if (new_size > m_Capacity)
				Allocate(ME::Core::Algorithm::Max(m_Capacity * ARR_RESIZE_MULTIPLYER, new_size));
			Ptr targetPtr = m_Data + offset;

			SIZE_T old_size = m_Size;

			for (SIZE_T i = 0; i < count; ++i)
			{
				m_Allocator.Construct(&m_Data[new_size - 1 - i], std::move(m_Data[old_size - 1 - i]));
				m_Allocator.Destroy(&m_Data[old_size - 1 - i]);
			}

			for (SIZE_T i = old_size - 1 - count; i >= offset; --i)
			{
				m_Data[i + count] = std::move(m_Data[i]);
			}

			m_Size = new_size;
		}

		Iterator PInsert(ConstIterator position, const SIZE_T count, const DataType& value)
		{
			SIZE_T currentPos = position - Begin();
			Reserve(m_Size + count);
			Iterator result = Begin() + currentPos;

			for (SIZE_T i = m_Size; i > currentPos; --i)
				PEmplaceBack(std::move(operator[](i - 1)));
            for (SIZE_T i = 0; i < count; ++i)
				PEmplace(position + i, value);
			return result;
		}

		template<typename InputIt>
		Iterator PInsert(ConstIterator position, InputIt first, InputIt last)
		{
			SIZE_T count = last - first;
			SIZE_T currentPos = position - Begin();
			Reserve(m_Size + count);
			Iterator result = Begin() + currentPos;
			for (SIZE_T i = m_Size; i > currentPos; --i)
				PEmplaceBack(std::move(operator[](i - 1)));
			for (SIZE_T i = 0; i < count; ++i)
				PEmplace(position + i, *(first + i));
			return result;
		}

		Iterator PInsert(ConstIterator position, const std::initializer_list<DataType>& list)
		{
			SIZE_T currentPos = position - Begin();
			Reserve(m_Size + list.size());
			Iterator result = Begin() + currentPos;
			for (SIZE_T i = m_Size; i > currentPos; --i)
				PEmplaceBack(std::move(operator[](i - 1)));
			for (SIZE_T i = 0; i < list.size(); ++i)
				PEmplace(position + i, list.size() + i);
			return result;
		}

		void PPopBack()
		{
			if (m_Size > 0)
			{
				m_Size--;
				m_Allocator.Destroy(&m_Data[m_Size]);
			}
		}

		Iterator PErase(Iterator it)
		{
			Ptr wherePtr = it.m_Ptr;

			for (Ptr pos = wherePtr; pos < m_Data + m_Size - 1; ++pos)
				*pos = std::move(*(pos + 1));

			m_Allocator.Destroy(m_Data + m_Size - 1);

			--m_Size;

			return Iterator(wherePtr);
		}

		Iterator PErase(ConstIterator it)
		{
			Ptr wherePtr = it.Ptr();

			for (Ptr pos = wherePtr; pos < m_Data + m_Size - 1; ++pos)
				*pos = std::move(*(pos + 1));

			m_Allocator.Destroy(m_Data + m_Size - 1);

			--m_Size;

			return Iterator(wherePtr);
		}

		Iterator PErase(Iterator first, Iterator last)
		{
			SIZE_T pos = first - Begin();
			SIZE_T count = last - first;

			for (SIZE_T i = pos; i < pos + count; ++i)
				m_Allocator.Destroy(m_Data + i);

			for (SIZE_T i = pos + count; i < m_Size; ++i)
				m_Data[pos + (i - (pos + count))] = std::move(m_Data[i]);

			m_Size -= count;
			return Begin() + pos;
		}

		Iterator PErase(ConstIterator first, ConstIterator last)
		{
			SIZE_T pos = first - Begin();
			SIZE_T count = last - first;

			for (SIZE_T i = pos; i < pos + count; ++i)
				m_Allocator.Destroy(m_Data + i);

			for (SIZE_T i = pos + count; i < m_Size; ++i)
				m_Data[pos + (i - (pos + count))] = std::move(m_Data[i]);

			m_Size -= count;
			return Begin() + pos;
		}

		bool PReserve(SIZE_T size)
		{
			if (size <= m_Capacity)
				return false;

			Allocate(size);

			return true;
		}

		void PShrink()
		{
			if constexpr(!std::is_trivially_destructible_v<T>)
				for (SIZE_T i = m_Size; i < m_Capacity; ++i)
    			    m_Allocator.Destroy(&m_Data[i]);
			m_Allocator.Deallocate(m_Data + m_Size, m_Capacity - m_Size);
			m_Capacity = m_Size;
			m_Data = m_Capacity <= 0 ? nullptr : m_Data;
		}

		void PAppend(const Array& array)
		{
			SIZE_T new_size = m_Size + array.Size();
			if (new_size > m_Capacity) {
				Allocate(ME::Core::Algorithm::Max(m_Capacity * ARR_RESIZE_MULTIPLYER, new_size));
			}

			for (SIZE_T i = 0; i < array.Size(); ++i) {
				m_Allocator.Construct(&m_Data[m_Size + i], array.m_Data[i]);
			}
			m_Size = new_size;
		}

		void PAppend(Array&& array)
		{
			SIZE_T new_size = m_Size + array.Size();
			if (new_size > m_Capacity) {
				Allocate(ME::Core::Algorithm::Max(m_Capacity * ARR_RESIZE_MULTIPLYER, new_size));
			}

			for (SIZE_T i = 0; i < array.Size(); ++i) {
				m_Allocator.Construct(&m_Data[m_Size + i], std::move(array.m_Data[i]));
			}

			m_Size = new_size;
			array.m_Size = 0;
			array.m_Capacity = 0;
			array.m_Data = nullptr;
		}

	private:
		AllocatorType m_Allocator;

	private:
		DataType* m_Data;

		SIZE_T m_Size;
		SIZE_T m_Capacity;
	};

}