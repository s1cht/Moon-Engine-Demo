#include "Hasher.hpp"
#include <xxhash.h>

namespace ME::Core::Memory
{
	CORE_API SIZE_T Hash64(const void* input, SIZE_T length, uint64 seed)
	{
		return XXH64(input, length, seed);
	}

}
