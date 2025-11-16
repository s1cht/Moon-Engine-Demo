#pragma once
#include "Core/Memory/Allocators/Allocator.hpp"
#include "Core/Utility/Logging/Logger.hpp"
#include "Core/Containers/String/StringShared.hpp"

namespace ME::Core
{
	template<typename type, class allocator>
	class PString;

	template<typename type>
	class PStringView
	{
	public:
		using DataType = type;
		using PtrType = const DataType*;
		using RefType = const DataType&;

	public:
		constexpr PStringView() : m_Data(nullptr), m_Size(0) {}

		constexpr PStringView(const asciichar* data)
			: m_Data(CONVERT_TEXT_UTF8(data)), m_Size(data ? GetStringSize(CONVERT_TEXT_UTF8(data)) : 0)
	    {
            static_assert(sizeof(DataType) == 1, "This constructor is available for only UTF8 string!");
		}

		constexpr PStringView(const asciichar* data, SIZE_T size)
			: m_Data(data), m_Size(data ? GetStringSize(data) : 0)
	    {
            static_assert(sizeof(DataType) == 1, "This constructor is available for only UTF8 string!");
		}

		constexpr PStringView(PtrType data)
			: m_Data(data), m_Size(data ? GetStringSize(data) : 0)
	    {}

		constexpr PStringView(PtrType data, SIZE_T size)
			: m_Data(data), m_Size(size)
	    {}

		template<SIZE_T initSize, class allocator>
		PStringView(const PString<DataType, allocator>& str)
			: m_Data(str.GetString()), m_Size(str.GetSize())
	    {}

		constexpr PStringView(const PStringView& other)
			: m_Data(other.m_Data), m_Size(other.m_Size)
	    {}

	public:
		inline constexpr SIZE_T Size() const noexcept { return m_Size; }
		inline constexpr PtrType String() const noexcept { return m_Data; }

		constexpr PtrType begin() const { return m_Data; }
		constexpr PtrType end() const { return m_Data + m_Size; }

		constexpr RefType operator[](SIZE_T index) const
		{
			return m_Data[index];
		}

		constexpr bool operator==(const DataType* str) const
		{
			if (m_Size != GetStringSize(str)) return false;
			for (size_t i = 0; i < m_Size; ++i)
				if (m_Data[i] != str[i]) 
					return false;
			return true;
		}

		constexpr bool operator==(const PStringView& other) const
		{
			if (m_Size != other.m_Size) return false;
			for (size_t i = 0; i < m_Size; ++i)
				if (m_Data[i] != other.m_Data[i])
					return false;
			return true;
		}

		constexpr bool operator!=(const PStringView& other) const
		{
			return !(*this == other);
		}

		PStringView Substring(SIZE_T start, SIZE_T length = 0) const
		{
			ME_CORE_ASSERT(start <= m_Size, "Start index out of range!");
			SIZE_T remaining = m_Size - start;
			SIZE_T newSize = (length > remaining) ? remaining : length;
			return PStringView(m_Data + start, newSize);
		}

		template<class allocator = ME::Core::Memory::Allocator<type>>
		PString<DataType, allocator> ToString() const
		{
			return PString<DataType, allocator>(m_Data, m_Size);
		}

	private:
		PtrType m_Data;
		SIZE_T m_Size;
	};

	typedef PStringView<char8> StringView; // UTF-8
	typedef PStringView<wchar> WideStringView; // UTF-16
}

ME_FMT_FORMATTER_UTF8(ME::Core::StringView, "{}", reinterpret_cast<const char*>(ME_FMT_FORMATTER_METHOD(String)));
ME_FMT_FORMATTER_UTF16(ME::Core::WideStringView, "{}", ME_FMT_FORMATTER_METHOD(String));