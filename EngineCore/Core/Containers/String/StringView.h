#pragma once

#include "Core.h"
#include "Core/Misc/Assertion.h"
#include "Core/Memory/PawnMemory.h"

namespace Pawn
{
	template<typename type, SIZE_T initSize, class allocator>
	class PString;

	template<typename type>
	class PStringView
	{
	public:
		using DataType = type;
		using PtrType = const DataType*;
		using RefType = const DataType&;

	public:
		PStringView() : m_Data(nullptr), m_Size(0) {}

		PStringView(PtrType data)
			: m_Data(data), m_Size(data ? GetStringSize(data) : 0) {
		}

		PStringView(PtrType data, SIZE_T size)
			: m_Data(data), m_Size(size) {
		}

		template<SIZE_T initSize, class allocator>
		PStringView(const PString<DataType, initSize, allocator>& str)
			: m_Data(str.GetString()), m_Size(str.GetSize()) {
		}

		PStringView(const PStringView& other)
			: m_Data(other.m_Data), m_Size(other.m_Size) {
		}

	public:
		inline SIZE_T GetSize() const noexcept { return m_Size; }
		inline PtrType GetData() const noexcept { return m_Data; }

		PtrType begin() const { return m_Data; }
		PtrType end() const { return m_Data + m_Size; }

		RefType operator[](SIZE_T index) const
		{
			PE_CORE_ASSERT(index < m_Size, "Index out of range in StringView!");
			return m_Data[index];
		}

		bool operator==(const PStringView& other) const
		{
			if (m_Size != other.m_Size) return false;
			return memcmp(m_Data, other.m_Data, m_Size * sizeof(DataType)) == 0;
		}

		bool operator!=(const PStringView& other) const
		{
			return !(*this == other);
		}

		PStringView Substring(SIZE_T start, SIZE_T length = SIZE_MAX) const
		{
			PE_CORE_ASSERT(start <= m_Size, "Start index out of range!");
			SIZE_T remaining = m_Size - start;
			SIZE_T newSize = (length > remaining) ? remaining : length;
			return PStringView(m_Data + start, newSize);
		}

		template<SIZE_T initSize, class allocator>
		PString<DataType, initSize, allocator> ToString() const
		{
			return PString<DataType, initSize, allocator>(m_Data, m_Size);
		}

	private:
		static SIZE_T GetStringSize(PtrType str)
		{
			if (!str) return 0;
			SIZE_T size = 0;
			while (str[size] != '\0') size++;
			return size;
		}

	private:
		PtrType m_Data;
		SIZE_T m_Size;
	};

	using StringView = PStringView<uchar>;   // UTF-16
	using AnsiStringView = PStringView<ansichar>; // ANSI
}