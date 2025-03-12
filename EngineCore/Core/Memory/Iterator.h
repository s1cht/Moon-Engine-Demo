#pragma once

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

	};

}