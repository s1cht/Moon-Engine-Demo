#include "Hasher.hpp"
#include <xxhash.h>

namespace ME::Core::Memory
{
	COREAPI SIZE_T Hash64(const void* input, SIZE_T length, uint64 seed)
	{
		return XXH64(input, length, seed);
	}

    SIZE_T Hasher<class ME::Core::String>::operator()(const String& str, SIZE_T tableSize) const noexcept
    {
        return Hash64(str.String(), str.Size() * sizeof(char8), 0) % tableSize;
    }

    SIZE_T Hasher<class ME::Core::WideString>::operator()(const ME::Core::WideString& str, SIZE_T tableSize) const noexcept
    {
        return Hash64(str.String(), str.Size() * sizeof(wchar), 0) % tableSize;
    }
}
