#pragma once

#include "Core/CoreTypes.h"

namespace Pawn::Memory
{
	template <class T>
	class Reference
	{
	public:
		using DataType = T;
		using Ptr = DataType*;

	public:

	private:
		
		uint32 m_ReferenceCount = 1;
		Ptr m_Data {nullptr};
	};
}