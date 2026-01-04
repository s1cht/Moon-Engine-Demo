#include "StringUtils.hpp"

namespace ME::Core::Utils
{
    namespace Helper
    {
        constexpr char8 HexValues[16] = { 
            TEXT('0'), TEXT('1'), TEXT('2'), TEXT('3'), TEXT('4'), TEXT('5'), TEXT('6'), TEXT('7'),
            TEXT('8'), TEXT('9'), TEXT('A'), TEXT('B'), TEXT('C'), TEXT('D'), TEXT('E'), TEXT('F')
        };
        constexpr wchar HexValuesW[16] = { 
            L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', 
            L'8', L'9', L'A', L'B', L'C', L'D', L'E', L'F' 
        };

        ME::Core::String NumberToHex(uint8 number)
        {
            ME::Core::String hex = "00";
            hex[0] = HexValues[number >> 4];
            hex[1] = HexValues[number & 0x0F];
            return hex;
        }
    }

    ME::Core::String ConvertNumberToHex(int64 number)
    {
        return ConvertNumberToHex(static_cast<uint64>(number));
    }

    ME::Core::String ConvertNumberToHex(int32 number)
    {
        return ConvertNumberToHex(static_cast<uint32>(number));
    }

    ME::Core::String ConvertNumberToHex(int16 number)
    {
        return ConvertNumberToHex(static_cast<uint16>(number));
    }

    ME::Core::String ConvertNumberToHex(int8 number)
    {
        return ConvertNumberToHex(static_cast<uint8>(number));
    }

    ME::Core::String ConvertNumberToHex(uint64 number)
    {
        ME::Core::String hex7 = Helper::NumberToHex(static_cast<uint8>(number >> 56));
        ME::Core::String hex6 = Helper::NumberToHex(static_cast<uint8>((number >> 48) & 0xFF));
        ME::Core::String hex5 = Helper::NumberToHex(static_cast<uint8>((number >> 40) & 0xFF));
        ME::Core::String hex4 = Helper::NumberToHex(static_cast<uint8>((number >> 32) & 0xFF));
        ME::Core::String hex3 = Helper::NumberToHex(static_cast<uint8>((number >> 24) & 0xFF));
        ME::Core::String hex2 = Helper::NumberToHex(static_cast<uint8>((number >> 16) & 0xFF));
        ME::Core::String hex1 = Helper::NumberToHex(static_cast<uint8>((number >> 8) & 0xFF));
        ME::Core::String hex0 = Helper::NumberToHex(static_cast<uint8>(number & 0xFF));
        return hex7 + hex6 + hex5 + hex4 + hex3 + hex2 + hex1 + hex0;
    }

    ME::Core::String ConvertNumberToHex(uint32 number)
    {
        ME::Core::String hex3 = Helper::NumberToHex(static_cast<uint8>(number >> 24));
        ME::Core::String hex2 = Helper::NumberToHex(static_cast<uint8>((number >> 16) & 0xFF));
        ME::Core::String hex1 = Helper::NumberToHex(static_cast<uint8>((number >> 8) & 0xFF));
        ME::Core::String hex0 = Helper::NumberToHex(static_cast<uint8>(number & 0xFF));
        return hex3 + hex2 + hex1 + hex0;
    }

    ME::Core::String ConvertNumberToHex(uint16 number)
    {
        ME::Core::String hex1 = Helper::NumberToHex(static_cast<uint8>(number >> 8));
        ME::Core::String hex0 = Helper::NumberToHex(static_cast<uint8>(number & 0x00FF));
        return hex1 + hex0;
    }

    ME::Core::String ConvertNumberToHex(uint8 number)
    {
        return Helper::NumberToHex(number);
    }

    uint64 ConvertHexToNumber(const ME::Core::String& hex)
    {
        uint64 result = 0;

        for (SIZE_T i = 0; i < hex.Size(); ++i)
        {
            uint8 value = 0;
            char8 c = hex[i];
            if (c >= TEXT('0') && c <= TEXT('9')) value = c - '0';
            else if (c >= TEXT('A') && c <= TEXT('F')) value = 10 + (c - 'A');
            else if (c >= TEXT('a') && c <= TEXT('f')) value = 10 + (c - 'a');
            else return ~0ull;
            result = (result << 4) | value;
        }

        return result;
    }
}