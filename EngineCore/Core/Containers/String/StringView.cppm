module;

#include "Core.h"
#include "Core/Misc/Assertion.h"

export module Pawn.Core.Container.StringView;

import Pawn.Core.Container.StringShared;

export namespace Pawn::Core::Containers
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
		PStringView() : m_Data(nullptr), m_Size(0) {}

		PStringView(PtrType data)
			: m_Data(data), m_Size(data ? GetStringSize(data) : 0) {
		}

		PStringView(PtrType data, SIZE_T size)
			: m_Data(data), m_Size(size) {
		}

		template<SIZE_T initSize, class allocator>
		PStringView(const PString<DataType, allocator>& str)
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

		bool operator==(const DataType* str) const
		{
			if (m_Size != GetStringSize(str)) return false;
			return memcmp(m_Data, str, m_Size * sizeof(DataType)) == 0;
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

		template<class allocator>
		PString<DataType, allocator> ToString() const
		{
			return PString<DataType, allocator>(m_Data, m_Size);
		}

	private:
		PtrType m_Data;
		SIZE_T m_Size;
	};

	typedef PStringView<uchar> StringView;   // UTF-16
	typedef PStringView<ansichar> AnsiStringView; // ANSI
}