#pragma once

#include "Core/Core.h"
#include <type_traits>

namespace Pawn 
{
	template<typename T>
	struct Vector2
	{
		PE_CORE_ASSERT(std::is_floating_point<T>, "")

		union
		{
			struct
			{

			};
		
		};

	};

};

