#pragma once

#include "Core.hpp"
#include "Core/Utils/Logging/Logger.hpp"
#include "Core/Memory/Memory.hpp"
#include "Core/Memory/Allocator.hpp"
#include "Core/Containers/String/StringShared.hpp"

#include <bitset>

#define STR_RESIZE_MULTIPLYER 2

namespace Pawn::Core::Containers
{
	template<typename type>
	class PStringView;

	template<typename _String>
	class StringIterator
	{
	public:
		using DataType = typename _String::DataType;
		using PtrType = DataType*;
		using RefType = DataType&;

	public:
		StringIterator() : m_Ptr(nullptr) {}
		StringIterator(PtrType ptr) : m_Ptr(ptr) {}

	public:

		StringIterator operator-(const SIZE_T& index)
		{
			StringIterator it = *this;
			it -= index;
			return it;
		}

		StringIterator operator+(const SIZE_T& index)
		{
			StringIterator it = *this;
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

		StringIterator& operator++()
		{
			m_Ptr++;
			return (*this);
		}

		StringIterator operator++(int)
		{
			StringIterator temp = (*this);
			m_Ptr++;
			return temp;
		}

		StringIterator& operator--()
		{
			m_Ptr--;
			return (*this);
		}

		StringIterator operator--(int)
		{
			StringIterator temp = (*this);
			m_Ptr--;
			return temp;
		}


		SIZE_T operator-(const StringIterator& it)
		{
			return m_Ptr - it.m_Ptr;
		}

		bool operator!=(const StringIterator& it)
		{
			return (m_Ptr != it.m_Ptr);
		}

		bool operator==(const StringIterator& it)
		{
			return (m_Ptr == it.m_Ptr);
		}

		StringIterator& operator= (const StringIterator& it)
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

	template<typename type, class allocator = Pawn::Core::Memory::Allocator<type>>
	class PString
	{
	public:
		using DataType = type;
		using ReturnType = DataType;
		using Ptr = DataType*;
		using Iterator = StringIterator<PString>;
		using AllocatorType = allocator;

	public:
		PString()
			: m_Data(nullptr), m_Size(0), m_Capacity(10), m_Allocator(AllocatorType())
		{
			Allocate(m_Capacity);
		}

		PString(const DataType* str)
			: m_Allocator(AllocatorType())
		{
			m_Size = GetStringSize(str);
			m_Capacity = m_Size + 1;
			m_Data = nullptr;

			Allocate(m_Capacity);
			CopyString(str, m_Size);
			m_Data[m_Size] = '\0';
		}

		PString(const DataType* str, SIZE_T size)
			: m_Allocator(AllocatorType())
		{
			m_Size = size;
			m_Capacity = m_Size + 1;
			m_Data = nullptr;

			Allocate(m_Capacity);
			CopyString(str, size);
			m_Data[m_Size] = '\0';
		}

		PString(const PString& other)
			: m_Allocator(AllocatorType())
		{
			m_Size = other.m_Size;
			m_Capacity = other.m_Capacity;
			m_Data = nullptr;

			Allocate(m_Capacity);
			CopyString(other.m_Data, m_Size);
			m_Data[m_Size] = '\0';
		}

		PString(PString&& other) noexcept
			: m_Allocator(other.m_Allocator), m_Size(other.m_Size), m_Capacity(other.m_Capacity), m_Data(other.m_Data)
		{
			other.m_Size = 0;
			other.m_Capacity = 0;
			other.m_Data = nullptr;
		}

		~PString()
		{
			if (m_Data)
			{
				m_Allocator.Deallocate(m_Data, m_Capacity * sizeof(DataType));
				m_Data = nullptr;
			}
		}

	public:
		inline SIZE_T GetSize() const noexcept { return m_Size; }
		inline SIZE_T GetCapacity() const noexcept { return m_Capacity; }
		inline const DataType* GetString() const noexcept { return m_Data; }

		inline Iterator begin() { return Iterator(m_Data); }
		inline Iterator end() { return Iterator(m_Data + m_Size); }
		inline Iterator Begin() { return Iterator(m_Data); }
		inline Iterator End() { return Iterator(m_Data + m_Size); }

	public:
		PStringView<type> ToStringView() const
		{
			return PStringView<type>(m_Data, m_Size);
		}

		operator PStringView<DataType>() const
		{
			return PStringView<DataType>(m_Data, m_Size);
		}

	public:

		PString& operator=(const DataType* str)
		{
			Clear();
			Assign(str);
			return *this;
		}

		PString& operator=(const DataType& str)
		{
			Clear();
			AddChar(str);
			return *this;
		}

		PString& operator=(const PString& other)
		{
			if (this != &other)
			{
				if (m_Data)
				{
					m_Allocator.Deallocate(m_Data, m_Capacity * sizeof(DataType));
				}
				m_Allocator = other.m_Allocator;
				m_Size = other.m_Size;
				m_Capacity = other.m_Capacity;
				m_Data = nullptr;

				Allocate(m_Capacity);
				CopyString(other.m_Data, m_Size);
			}
			return *this;
		}

		PString operator+(const DataType* str) const
		{
			PString result = *this;
			result.AddToString(str, GetStringSize(str));
			return result;
		}

		PString operator+(const PString& str) const
		{
			PString result = *this;
			result.AddToString(str.GetString(), str.GetSize());
			return result;
		}

		PString operator+(PStringView<DataType> str) const
		{
			PString result = *this;
			result.AddToString(str.GetData(), str.GetSize());
			return result;
		}

		PString operator+(const DataType& character) const
		{
			PString result = *this;
			result.AddChar(character);
			return result;
		}

		PString& operator+=(const PString& str)
		{
			return AddToString(str.GetString(), str.GetSize());
		}

		PString operator+=(PStringView<DataType> str) const
		{
			return AddToString(str.GetData(), str.GetSize());
		}

		PString& operator+=(const DataType* str)
		{
			return AddToString(str, GetStringSize(str));
		}

		PString& operator+=(const DataType& character)
		{
			return AddChar(character);
		}

		bool operator==(const PString& str) const
		{
			if (str.GetSize() != m_Size)
				return false;

			if constexpr (std::is_trivially_copyable_v<DataType>)
			{
				return memcmp(m_Data, str.m_Data, m_Size * sizeof(DataType)) == 0;
			}
			else
			{
				for (SIZE_T i = 0; i < m_Size; i++)
				{
					if (m_Data[i] != str.m_Data[i])
						return false;
				}
				return true;
			}
		}

		ReturnType& operator[](const SIZE_T index) noexcept
		{
			PE_CORE_ASSERT(index < m_Size, "Index in array is out of range!");
			return m_Data[index];
		}

		const ReturnType& operator[](const SIZE_T index) const noexcept
		{
			PE_CORE_ASSERT(index < m_Size, "Index in array is out of range!");
			return m_Data[index];
		}

	public:
		void Clear()
		{
			m_Size = 0;
			if (m_Data)
				m_Data[0] = '\0';
		}

		bool Resize(SIZE_T size)
		{
			if (size + 1 <= m_Capacity)
			{
				return false;
			}

			Allocate(size + 1);
			return true;
		}

		void PopBack()
		{
			if (m_Size > 0)
			{
				m_Size--;
				m_Data[m_Size] = '\0';
			}
		}

	public:
		inline bool Empty() const
		{
			return m_Size <= 0;
		}

	public:

		PString Substring(SIZE_T start, SIZE_T length = 0) const
		{
			PE_CORE_ASSERT(start <= m_Size, "Start index out of range!");
			SIZE_T remaining = m_Size - start;
			SIZE_T newSize = (length > remaining) ? remaining : length;
			return PString(m_Data + start, newSize);
		}

	public:
		inline SIZE_T Find(const PString& in, SIZE_T pos = 0) const
		{
			return PFind(in.GetString(), in.GetSize(), pos);
		}

		inline SIZE_T Find(const DataType* in, SIZE_T pos = 0) const
		{
			return PFind(in, GetStringSize(in), pos);
		}

		inline SIZE_T Find(const DataType* in, SIZE_T pos, SIZE_T n) const
		{
			return PFind(in, n, pos);
		}

		inline SIZE_T Find(DataType in, SIZE_T pos = 0) const
		{
			return PFind(const_cast<const DataType*>(&in), 1, pos);
		}


		inline 	SIZE_T FindFirst(const PString& in, SIZE_T pos = 0) const
		{
			return PFindFirst(in.GetString(), in.GetSize(), pos);
		}

		inline SIZE_T FindFirst(const DataType* in, SIZE_T pos = 0) const
		{
			return PFindFirst(in, GetStringSize(in), pos);
		}

		inline SIZE_T FindFirst(const DataType* in, SIZE_T pos, SIZE_T n) const
		{
			return PFindFirst(in, n, pos);
		}

		inline SIZE_T FindFirst(DataType in, SIZE_T pos = 0) const
		{
			return PFindFirst(const_cast<const DataType*>(&in), 1, pos);
		}


		inline SIZE_T FindFirstNot(const PString& in, SIZE_T pos = 0) const
		{
			return PFindFirstNot(in.GetString(), in.GetSize(), pos);
		}

		inline SIZE_T FindFirstNot(const DataType* in, SIZE_T pos = 0) const
		{
			return PFindFirstNot(in, GetStringSize(in), pos);
		}

		inline SIZE_T FindFirstNot(const DataType* in, SIZE_T pos, SIZE_T n) const
		{
			return PFindFirstNot(in, n, pos);
		}

		inline SIZE_T FindFirstNot(DataType in, SIZE_T pos = 0) const
		{
			return PFindFirstNot(const_cast<const DataType*>(&in), 1, pos);
		}


		inline SIZE_T FindLast(const PString& in, SIZE_T pos = 0) const
		{
			pos = pos == 0 ? m_Size : pos;
			return PFindLast(in.GetString(), in.GetSize(), pos);
		}

		inline SIZE_T FindLast(const DataType* in, SIZE_T pos = 0) const
		{
			pos = pos == 0 ? m_Size : pos;
			return PFindLast(in, GetStringSize(in), pos);
		}

		inline SIZE_T FindLast(const DataType* in, SIZE_T pos, SIZE_T n) const
		{
			pos = pos == 0 ? m_Size : pos;
			return PFindLast(in, n, pos);
		}

		inline SIZE_T FindLast(DataType in, SIZE_T pos = 0) const
		{
			pos = pos == 0 ? m_Size : pos;
			return PFindLast(const_cast<const DataType*>(&in), 1, pos);
		}


		inline SIZE_T FindLastNot(const PString& in, SIZE_T pos = 0) const
		{
			pos = pos == 0 ? m_Size : pos;
			return PFindLastNot(in.GetString(), in.GetSize(), pos);
		}

		inline SIZE_T FindLastNot(const DataType* in, SIZE_T pos = 0) const
		{
			pos = pos == 0 ? m_Size : pos;
			return PFindLastNot(in, GetStringSize(in), pos);
		}

		inline SIZE_T FindLastNot(const DataType* in, SIZE_T pos, SIZE_T n) const
		{
			pos = pos == 0 ? m_Size : pos;
			return PFindLastNot(in, n, pos);
		}

		inline SIZE_T FindLastNot(DataType in, SIZE_T pos = 0) const
		{
			pos = pos == 0 ? m_Size : pos;
			return PFindLastNot(const_cast<const DataType*>(&in), 1, pos);
		}

	public:
		void Assign(const DataType*& ptr)
		{
			Assign(ptr, GetStringSize(ptr));
		}

		void Assign(const DataType*& ptr, SIZE_T size)
		{
			PE_ASSERT(ptr != nullptr && size != 0, TEXT("Assigning nullptr or ptr with size == 0 to string!"));
			if (size + 1 > m_Capacity)
				Allocate(size + 1);

			CopyString(ptr, size);
			m_Size = size;
			m_Data[m_Size] = '\0';
		}

	private:
		void Allocate(SIZE_T newCapacity)
		{
			Ptr newBlock = m_Allocator.Allocate(newCapacity * sizeof(DataType));
			SIZE_T copySize = m_Size < newCapacity - 1 ? m_Size : newCapacity - 1;

			if (m_Data)
			{
				for (SIZE_T i = 0; i < copySize; i++)
					newBlock[i] = m_Data[i];
				m_Allocator.Deallocate(m_Data, m_Capacity * sizeof(DataType));
			}

			m_Data = newBlock;
			m_Capacity = newCapacity;
			if (copySize < m_Size)
				m_Size = copySize;
			m_Data[m_Size] = '\0';
		}

		void CopyString(const DataType* str, SIZE_T size)
		{
			memcpy(m_Data, str, size * sizeof(DataType));
		}

		PString& AddToString(const DataType* str, SIZE_T size)
		{
			if (m_Size + size + 1 > m_Capacity)
				Allocate((SIZE_T)((m_Size + size + 1) * STR_RESIZE_MULTIPLYER));

			for (SIZE_T i = 0; i < size; i++)
				m_Data[m_Size + i] = str[i];

			m_Size += size;
			m_Data[m_Size] = '\0';
			return *this;
		}

		PString& AddChar(const DataType& character)
		{
			if (m_Size + 1 >= m_Capacity)
				Allocate((SIZE_T)(m_Capacity * STR_RESIZE_MULTIPLYER));

			if (m_Size == 0)
			{
				m_Data[0] = character;
				m_Size++;
				m_Data[m_Size] = '\0';
				return *this;
			}

			m_Data[m_Size] = character;
			m_Size++;
			m_Data[m_Size] = '\0';
			return *this;
		}

	private:
		// Find string algorithm.
		// Needle - data to find in string
		SIZE_T PFind(const DataType* needle, SIZE_T needleSize, SIZE_T startAt) const
		{
			if (needleSize == 0 || startAt >= m_Size || needleSize > m_Size - startAt)
				return m_Size;

			const Ptr firstChar = needle[0];
			SIZE_T limit = m_Size - needleSize;

			for (SIZE_T i = startAt; i <= limit; ++i)
			{
				if (m_Data[i] == firstChar)
				{
					bool match = true;
					for (SIZE_T j = 1; j < needleSize; ++j)
					{
						if (m_Data[i + j] != needle[j])
						{
							match = false;
							break;
						}
					}
					if (match)
						return i;
				}
			}
			return m_Size;
		}

		SIZE_T PFindFirst(const DataType* needle, SIZE_T needleSize, SIZE_T startAt) const
		{
			if (!needle || needleSize == 0 || startAt >= m_Size)
				return m_Size;

			if (needleSize <= 16)
			{
				for (SIZE_T i = startAt; i < m_Size; ++i)
				{
					for (SIZE_T j = 0; j < needleSize; ++j)
					{
						if (m_Data[i] == needle[j])
							return i;
					}
				}
			}
			else
			{
				if constexpr (sizeof(DataType) == 1)
				{
					bool table[256] = { false };
					for (SIZE_T j = 0; j < needleSize; ++j)
						table[static_cast<uint8>(needle[j])] = true;

					for (SIZE_T i = startAt; i < m_Size; ++i)
						if (table[static_cast<uint8>(m_Data[i])])
							return i;
				}
				else if constexpr (sizeof(DataType) == 2)
				{
					std::bitset<65536> table;
					for (SIZE_T j = 0; j < needleSize; ++j)
						table.set(static_cast<uint16>(needle[j]));

					for (SIZE_T i = startAt; i < m_Size; ++i)
						if (table.test(static_cast<uint16>(m_Data[i])))
							return i;
				}
				else
				{
					PE_CORE_ERROR(TEXT("Unsupported DataType size ({0})! Only ASCII/UTF-8 (1 byte) and UTF-16 (2 bytes) are supported."), sizeof(DataType));
					return m_Size;
				}
			}

			return m_Size;
		}

		SIZE_T PFindFirstNot(const DataType* needle, SIZE_T needleSize, SIZE_T startAt) const
		{
			if (!needle || needleSize == 0 || startAt >= m_Size)
				return m_Size;

			if (needleSize <= 16)
			{
				for (SIZE_T i = startAt; i < m_Size; ++i)
				{
					bool found = false;
					for (SIZE_T j = 0; j < needleSize; ++j)
					{
						if (m_Data[i] == needle[j])
						{
							found = true;
							break;
						}
					}
					if (!found)
						return i;
				}
			}
			else
			{
				if constexpr (sizeof(DataType) == 1)
				{
					bool table[256] = { false };
					for (SIZE_T j = 0; j < needleSize; ++j)
						table[static_cast<uint8>(needle[j])] = true;

					for (SIZE_T i = startAt; i < m_Size; ++i)
						if (!table[static_cast<uint8>(m_Data[i])])
							return i;
				}
				else if constexpr (sizeof(DataType) == 2)
				{
					std::bitset<65536> table;
					for (SIZE_T j = 0; j < needleSize; ++j)
						table.set(static_cast<uint16>(needle[j]));

					for (SIZE_T i = startAt; i < m_Size; ++i)
						if (!table.test(static_cast<uint16>(m_Data[i])))
							return i;
				}
				else
				{
					PE_CORE_ERROR(TEXT("Unsupported DataType size ({0})! Only ASCII/UTF-8 (1 byte) and UTF-16 (2 bytes) are supported."), sizeof(DataType));
					return m_Size;
				}
			}

			return m_Size;
		}

		SIZE_T PFindLast(const DataType* needle, SIZE_T needleSize, SIZE_T startAt) const
		{
			if (!needle || needleSize == 0 || startAt >= m_Size)
				return m_Size;

			if (needleSize <= 16)
			{
				for (SIZE_T i = startAt; i != static_cast<SIZE_T>(-1); --i)
				{
					for (SIZE_T j = 0; j < needleSize; ++j)
					{
						if (m_Data[i] == needle[j])
							return i;
					}
				}
			}
			else
			{
				if constexpr (sizeof(DataType) == 1)
				{
					bool table[256] = { false };
					for (SIZE_T j = 0; j < needleSize; ++j)
						table[static_cast<uint8>(needle[j])] = true;

					for (SIZE_T i = startAt; i != static_cast<SIZE_T>(-1); --i)
						if (table[static_cast<uint8>(m_Data[i])])
							return i;
				}
				else if constexpr (sizeof(DataType) == 2)
				{
					std::bitset<65536> table;
					for (SIZE_T j = 0; j < needleSize; ++j)
						table.set(static_cast<uint16>(needle[j]));

					for (SIZE_T i = startAt; i != static_cast<SIZE_T>(-1); --i)
						if (table.test(static_cast<uint16>(m_Data[i])))
							return i;
				}
				else
				{
					PE_CORE_ERROR(TEXT("Unsupported DataType size ({0})! Only ASCII/UTF-8 (1 byte) and UTF-16 (2 bytes) are supported."), sizeof(DataType));
					return m_Size;
				}
			}

			return m_Size;
		}

		SIZE_T PFindLastNot(const DataType* needle, SIZE_T needleSize, SIZE_T startAt) const
		{
			if (!needle || needleSize == 0 || startAt >= m_Size)
				return m_Size;

			if (needleSize <= 16)
			{
				for (SIZE_T i = startAt; i != static_cast<SIZE_T>(-1); --i)
				{
					bool found = false;
					for (SIZE_T j = 0; j < needleSize; ++j)
					{
						if (m_Data[i] == needle[j])
						{
							found = true;
							break;
						}
					}
					if (found)
						return i;
				}
			}
			else
			{
				if constexpr (sizeof(DataType) == 1)
				{
					bool table[256] = { false };
					for (SIZE_T j = 0; j < needleSize; ++j)
						table[static_cast<uint8>(needle[j])] = true;

					for (SIZE_T i = startAt; i != static_cast<SIZE_T>(-1); --i)
						if (!table[static_cast<uint8>(m_Data[i])])
							return i;
				}
				else if constexpr (sizeof(DataType) == 2)
				{
					std::bitset<65536> table;
					for (SIZE_T j = 0; j < needleSize; ++j)
						table.set(static_cast<uint16>(needle[j]));

					for (SIZE_T i = startAt; i != static_cast<SIZE_T>(-1); --i)
						if (!table.test(static_cast<uint16>(m_Data[i])))
							return i;
				}
				else
				{
					PE_CORE_ERROR(TEXT("Unsupported DataType size ({0})! Only ASCII/UTF-8 (1 byte) and UTF-16 (2 bytes) are supported."), sizeof(DataType));
					return m_Size;
				}
			}

			return m_Size;
		}

	private:
		AllocatorType m_Allocator;
		Ptr m_Data;
		SIZE_T m_Size;
		SIZE_T m_Capacity;

	};

	typedef PString<uchar, Pawn::Core::Memory::Allocator<uchar>> String;
	typedef PString<ansichar, Pawn::Core::Memory::Allocator<ansichar>> AnsiString;

	//template<typename convertTo, SIZE_T strSize = 10, typename allocator = Allocator<uchar>>
	//CORE_API PString<strSize, allocator> TToString(convertTo value);

	CORE_API String ToString(int8 value);
	CORE_API String ToString(int16 value);
	CORE_API String ToString(int32 value);
	CORE_API String ToString(int64 value);

	CORE_API String ToString(uint8 value);
	CORE_API String ToString(uint16 value);
	CORE_API String ToString(uint32 value);
	CORE_API String ToString(uint64 value);

	CORE_API String ToString(float32 value);
	CORE_API String ToString(float64 value);

	CORE_API String ToString(bool value);
	CORE_API String ToString(const String& value);
	CORE_API String ToString(const uchar& value);


	CORE_API String operator+(const uchar* str1, const String& str2);
	CORE_API String operator+(const uchar& str1, const String& str2);

	CORE_API AnsiString ToAnsiString(int8 value);
	CORE_API AnsiString ToAnsiString(int16 value);
	CORE_API AnsiString ToAnsiString(int32 value);
	CORE_API AnsiString ToAnsiString(int64 value);

	CORE_API AnsiString ToAnsiString(uint8 value);
	CORE_API AnsiString ToAnsiString(uint16 value);
	CORE_API AnsiString ToAnsiString(uint32 value);
	CORE_API AnsiString ToAnsiString(uint64 value);

	CORE_API AnsiString ToAnsiString(float32 value);
	CORE_API AnsiString ToAnsiString(float64 value);

	CORE_API AnsiString ToAnsiString(bool value);
	CORE_API AnsiString ToAnsiString(const AnsiString& value);
	CORE_API AnsiString ToAnsiString(const ansichar& value);

	CORE_API int64 StringToInt(const String& str, SIZE_T* pos, int8 base = 10);
	CORE_API uint64 StringToUint(const String& str, SIZE_T* pos, int8 base = 10);
	CORE_API float64 StringToFloat(const String& str, SIZE_T* pos);

	CORE_API AnsiString StringToAnsiString(const String& str);
	CORE_API String AnsiStringToString(const AnsiString& str);

	CORE_API AnsiString operator+(const ansichar* str1, const AnsiString& str2);
	CORE_API AnsiString operator+(const ansichar& str1, const AnsiString& str2);
}