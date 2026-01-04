#pragma once
#include "Core.hpp"
#include "BasicString.hpp"

namespace ME::Core::Utils
{
    COREAPI ME::Core::String ConvertNumberToHex(int64 number);
    COREAPI ME::Core::String ConvertNumberToHex(int32 number);
    COREAPI ME::Core::String ConvertNumberToHex(int16 number);
    COREAPI ME::Core::String ConvertNumberToHex(int8 number);

    COREAPI ME::Core::String ConvertNumberToHex(uint64 number);
    COREAPI ME::Core::String ConvertNumberToHex(uint32 number);
    COREAPI ME::Core::String ConvertNumberToHex(uint16 number);
    COREAPI ME::Core::String ConvertNumberToHex(uint8 number);

    
    COREAPI uint64 ConvertHexToNumber(const ME::Core::String& hex);
}