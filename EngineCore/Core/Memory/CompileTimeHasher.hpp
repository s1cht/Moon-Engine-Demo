#pragma once
#include "Core/Types.hpp"
#include "Core/Containers/String/StringView.hpp"

namespace ME::Core::Memory
{
    namespace HashConstants
    {
        constexpr uint64 FNV_PRIME = 1099511628211ULL;
        constexpr uint64 FNV_OFFSET_BASIS = 14695981039346656037ULL;
    }

    constexpr uint64 HashFNV1(const char* data, SIZE_T length)
    {
        uint64 hash = HashConstants::FNV_OFFSET_BASIS;
        for (SIZE_T i = 0; i < length; ++i) 
        {
            hash ^= static_cast<uint64>(data[i]);
            hash *= HashConstants::FNV_PRIME;
        }
        return hash;
    }

    constexpr uint64 HashFNV1(const StringView& str)
    {
        uint64 hash = HashConstants::FNV_OFFSET_BASIS;
        for (SIZE_T i = 0; i < str.Size(); ++i)
        {
            hash ^= static_cast<uint64>(str.String()[i]);
            hash *= HashConstants::FNV_PRIME;
        }
        return hash;
    }
}
