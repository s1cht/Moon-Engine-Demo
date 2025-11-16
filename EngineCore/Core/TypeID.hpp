#pragma once
#include "Core/Memory/CompileTimeHasher.hpp"
#include <typeinfo>

namespace ME::Core
{
    template<typename Retval, uint64 Range>
    struct TypeIDGenerator
    {
    public:
        template<typename T>
        inline static Retval ID()
        {
            const std::type_info& info = typeid(T);
            return (info.hash_code() ^ (ME::Core::Memory::HashFNV1(info.name()) << 1)) % Range;
        }
    };
}