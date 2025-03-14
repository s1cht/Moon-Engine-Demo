#pragma once

#include "StringShared.h"

#include "Core.h"
#include "Core/Misc/Assertion.h"
#include "Core/Memory/PawnMemory.h"
#include "Core/Utils/Logging/Logger.h"

#define STR_RESIZE_MULTIPLYER 1.5

namespace Pawn
{
    template<typename _String>
    class StringIterator : public Pawn::Memory::BaseIterator<_String>
    {
    public:
        StringIterator() : Pawn::Memory::BaseIterator<_String>() {}
        StringIterator(Pawn::Memory::BaseIterator<_String>::PtrType ptr) : Pawn::Memory::BaseIterator<_String>(ptr) {}
    };

    template<typename type, SIZE_T initSize = 10, class allocator = Pawn::Memory::Allocator<type>>
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
            : m_Data(nullptr), m_Size(0), m_Capacity(initSize), m_Allocator(AllocatorType())
        {
            Allocate(m_Capacity);
        }

        PString(const uchar* str)
            : m_Allocator(AllocatorType())
        {
            m_Size = GetStringSize(str);
            m_Capacity = m_Size + 1;
            m_Data = nullptr;

            Allocate(m_Capacity);
            CopyString(str, m_Size);
            m_Data[m_Size] = TEXT('\0');
        }

        PString(const uchar* str, SIZE_T size)
            : m_Allocator(AllocatorType())
        {
            m_Size = size;
            m_Capacity = m_Size + 1;
            m_Data = nullptr;

            Allocate(m_Capacity);
            CopyString(str, size);
            m_Data[m_Size] = TEXT('\0');
        }

        PString(const PString& other)
            : m_Allocator(AllocatorType())
        {
            m_Size = other.m_Size;
            m_Capacity = other.m_Capacity;
            m_Data = nullptr;

            Allocate(m_Capacity);
            CopyString(other.m_Data, m_Size);
            m_Data[m_Size] = TEXT('\0');
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
        inline const uchar* GetString() const noexcept { return m_Data; }

        inline Iterator begin() const { return Iterator(m_Data); }
        inline Iterator end() const { return Iterator(m_Data + m_Size); }

        PString& operator=(const uchar* str)
        {
            Clear();
            Assign(str);
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

        PString operator+(const uchar* str) const
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

        PString operator+(const uchar& character) const
        {
            PString result = *this;
            result.AddChar(character);
            return result;
        }

        PString& operator+=(const PString& str)
        {
            return AddToString(str.GetString(), str.GetSize());
        }

        PString& operator+=(const uchar* str)
        {
            return AddToString(str, GetStringSize(str));
        }

        PString& operator+=(const uchar& character)
        {
            return AddChar(character);
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
                m_Data[0] = TEXT('\0');
        }

        bool Resize(SIZE_T size)
        {
            if (size + 1 <= m_Capacity)
            {
                m_Size = size;
                m_Data[m_Size] = TEXT('\0');
                return false;
            }

            Allocate(size + 1);
            m_Size = size;
            m_Data[m_Size] = TEXT('\0');
            return true;
        }

        void PopBack()
        {
            if (m_Size > 0)
            {
                m_Size--;
                m_Data[m_Size] = TEXT('\0');
            }
        }

    public:
        void Assign(const uchar*& ptr)
        {
            Assign(ptr, GetStringSize(ptr));
        }

        void Assign(const uchar*& ptr, SIZE_T size)
        {
            if (size + 1 > m_Capacity)
                Allocate(size + 1);

            CopyString(ptr, size);
            m_Size = size;
            m_Data[m_Size] = TEXT('\0');
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
            m_Data[m_Size] = TEXT('\0');
        }

        void CopyString(const uchar* str, SIZE_T size)
        {
            for (SIZE_T i = 0; i < size; i++)
                m_Data[i] = str[i];
        }

        PString& AddToString(const uchar* str, SIZE_T size)
        {
            if (m_Size + size + 1 > m_Capacity)
                Allocate((SIZE_T)((m_Size + size + 1) * STR_RESIZE_MULTIPLYER));

            for (SIZE_T i = 0; i < size; i++)
                m_Data[m_Size + i] = str[i];

            m_Size += size;
            m_Data[m_Size] = TEXT('\0');
            return *this;
        }

        PString& AddChar(const uchar& character)
        {
            if (m_Size + 1 >= m_Capacity)
                Allocate((SIZE_T)(m_Capacity * STR_RESIZE_MULTIPLYER));

            m_Data[m_Size] = character;
            m_Size++;
            m_Data[m_Size] = TEXT('\0');
            return *this;
        }

    private:
        AllocatorType m_Allocator;
        Ptr m_Data;
        SIZE_T m_Size;
        SIZE_T m_Capacity;

    };

    typedef PString<uchar, 10> String;
    typedef PString<ansichar, 10> AnsiString;

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
}

#define var_to_string(type, value) TEXT(" [") TEXT(#type) TEXT("] ") TEXT(#value) TEXT(" = ") + Pawn::ToString(value) + TEXT(";")
#define PE_NAMED_VARIABLE_TOSTRING(type, value, name) TEXT(" [") TEXT(#type) TEXT("] ") name TEXT(" = ") + Pawn::ToString(value) + TEXT(";")