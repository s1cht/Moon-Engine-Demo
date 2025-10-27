#pragma once

#include "Core.hpp"
#include "Core/Containers/String/StringView.hpp"

namespace ME::Core::Memory
{
	CORE_API SIZE_T Hash64(const void* input, SIZE_T length, uint64 seed);

	template <typename T>
	struct Hasher
	{
		using DataType = T;

	public:
		ME_NODISCARD inline SIZE_T operator()(const ME::Core::Containers::StringView& str, SIZE_T tableSize) const noexcept
		{
			return Hash64(str.GetData(), str.GetSize() * sizeof(uchar), 0) % tableSize;
		}

		ME_NODISCARD inline SIZE_T operator()(const T* data, SIZE_T tableSize) const noexcept
		{
			return Hash64(data, sizeof(T), 0) % tableSize;
		}

		ME_NODISCARD inline SIZE_T operator()(T data, SIZE_T tableSize) const noexcept
		{
			return Hash64(&data, sizeof(T), 0) % tableSize;
		}

		ME_NODISCARD inline SIZE_T operator()(const void* data, SIZE_T size, SIZE_T tableSize) const noexcept
		{
			return Hash64(data, size, 0) % tableSize;
		}

	};
}