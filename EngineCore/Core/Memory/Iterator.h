#pragma once

#include "pch.h"
#include "Core.h"

namespace Pawn::Memory
{
	template<typename Container>
	class CORE_API BaseIterator
	{
	public:
		using DataType = typename Container::DataType;
		using PtrType = DataType*;
		using RefType = DataType&;

	public:
		virtual ~BaseIterator()
		{
			m_Ptr = nullptr;
		}

	public:

		virtual BaseIterator& operator++()
		{
			m_Ptr++;
			return (*this);
		}

		virtual BaseIterator operator++(int)
		{
			BaseIterator temp = (*this);
			m_Ptr++;
			return temp;
		}

		virtual BaseIterator& operator--()
		{
			m_Ptr--;
			return (*this);
		}

		virtual BaseIterator operator--(int)
		{
			BaseIterator temp = (*this);
			m_Ptr--;
			return temp;
		}

		virtual RefType operator[] (SIZE_T index)
		{
			return *(m_Ptr + index);
		}

		virtual PtrType operator->()
		{
			return m_Ptr;
		}

		virtual bool operator!=(const BaseIterator& it)
		{
			return (m_Ptr != it.m_Ptr);
		}

		virtual bool operator==(const BaseIterator& it)
		{
			return (m_Ptr == it.m_Ptr);
		}

		virtual BaseIterator& operator= (const BaseIterator& it)
		{
			this->m_Ptr = it.m_Ptr;
			return *this;
		}

		virtual RefType operator*()
		{
			return *m_Ptr;
		}

	public:
		BaseIterator() : m_Ptr(nullptr) {};
		BaseIterator(PtrType ptr) : m_Ptr(ptr) {};

		PtrType m_Ptr;

	};

}