#pragma once

#include "Core.h"

namespace Pawn::Memory
{
	template<typename Container>
	class BaseIterator
	{
	public:
		using DataType = typename Container::DataType;
		using PtrType = DataType*;
		using RefType = DataType&;

	};

}