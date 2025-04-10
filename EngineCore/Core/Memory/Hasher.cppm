module;

#include "Core.h"

#define XXH_STATIC_LINKING_ONLY 
#define XXH_IMPLEMENTATION
#include <xxhash.h>


export module Pawn.Core.Memory.Hasher;

import Pawn.Core.Container.StringView;

export namespace Pawn::Core::Memory
{
	template <typename T>
	struct Hasher
	{
		using DataType = T;

	public:
		SIZE_T operator()(const Pawn::Core::Containers::StringView& str, SIZE_T tableSize) const
		{
			XXH64_hash_t hash = XXH64(str.GetData(), str.GetSize() * sizeof(uchar), 0);
			return static_cast<SIZE_T>(hash) % tableSize;
		}

		SIZE_T operator()(const T* data, SIZE_T tableSize) const
		{
			XXH64_hash_t hash = XXH64(data, sizeof(T), 0);
			return static_cast<SIZE_T>(hash) % tableSize;
		}

		SIZE_T operator()(T data, SIZE_T tableSize) const
		{
			XXH64_hash_t hash = XXH64(&data, sizeof(T), 0);
			return static_cast<SIZE_T>(hash) % tableSize;
		}

		SIZE_T operator()(const void* data, SIZE_T size, SIZE_T tableSize) const
		{
			XXH64_hash_t hash = XXH64(data, size, 0);
			return static_cast<SIZE_T>(hash) % tableSize;
		}

	};
}