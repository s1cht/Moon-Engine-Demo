#pragma once

#include "Core.hpp"
#include "Core/Containers/String.hpp"

namespace ME::Core::Memory
{
	COREAPI SIZE_T Hash64(const void* input, SIZE_T length, uint64 seed);

	template <typename T>
	struct Hasher
	{
		using DataType = T;

	public:
		ME_NODISCARD inline SIZE_T operator()(const ME::Core::StringView& str, SIZE_T tableSize) const noexcept
		{
			return Hash64(str.String(), str.Size() * sizeof(char8), 0) % tableSize;
		}

		ME_NODISCARD inline SIZE_T operator()(const DataType* data, SIZE_T tableSize) const noexcept
		{
			return Hash64(data, sizeof(DataType), 0) % tableSize;
		}

		ME_NODISCARD inline SIZE_T operator()(DataType data, SIZE_T tableSize) const noexcept
		{
			return Hash64(&data, sizeof(DataType), 0) % tableSize;
		}

		ME_NODISCARD inline SIZE_T operator()(const void* data, SIZE_T size, SIZE_T tableSize) const noexcept
		{
			return Hash64(data, size, 0) % tableSize;
		}
	};

	template<>
	struct COREAPI Hasher<class ME::Core::String>
	{
	public:
        ME_NODISCARD SIZE_T operator()(const ME::Core::String& str, SIZE_T tableSize) const noexcept;
    };

	template<>
	struct COREAPI Hasher<class ME::Core::WideString>
	{
	public:
        ME_NODISCARD SIZE_T operator()(const ME::Core::WideString& str, SIZE_T tableSize) const noexcept;
    };
}