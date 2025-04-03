

#pragma once

#include "Core.h"
#include <cmath>

namespace Pawn::Core::Math
{
	struct Quaternion
	{
		union 
		{
			struct
			{
				float32 w, x, y, z;
			};
		};
	};

}