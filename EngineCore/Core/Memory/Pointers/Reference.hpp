#pragma once

#include "Core/CoreTypes.h"

#include "ReferenceController.h"

namespace ME::Memory
{
	template <class T, Pointer::PtrMode Mode = Pointer::PtrMode::SingleThreaded>
	class Reference
	{
	public:
		using DataType = T;
		using Ptr = DataType*;

	public:
		Reference() : m_Data(nullptr) {};
		
		Reference(T* pointer) : m_Data(pointer), m_Reference(Pointer::CreateReferenceController<T, Mode>(pointer))
		{
		}


	private:
		template <class OtherT, Pointer::PtrMode OtherMode> friend class Reference;
		template <class OtherT, Pointer::PtrMode OtherMode> friend class WeakReference;

	private:
		Ptr m_Data {nullptr};
		Pointer::ReferenceHandler<Mode> m_Reference;

	};

	template <class T, Pointer::PtrMode Mode = Pointer::PtrMode::SingleThreaded>
	class WeakReference
	{
	public:
		using DataType = T;
		using Ptr = DataType*;

	private:
		template <class OtherT, Pointer::PtrMode OtherMode> friend class Reference;
		template <class OtherT, Pointer::PtrMode OtherMode> friend class WeakReference;

	private:
		Ptr m_Data{ nullptr };

		Pointer::ReferenceController<Mode> m_Reference;
	};
}