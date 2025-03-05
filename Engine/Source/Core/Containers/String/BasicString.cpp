#include "pch.h"
#include "BasicString.h"

namespace Pawn
{
    PAWN_API String ToString(int8 value)
    {
        return ToString(static_cast<int64>(value));
    }

    PAWN_API String ToString(int16 value)
    {
        return ToString(static_cast<int64>(value));
    }

    PAWN_API String ToString(int32 value)
    {
        return ToString(static_cast<int64>(value));
    }

    PAWN_API String ToString(int64 value)
    {
        if (value == 0) return TEXT("0");

        bool is_negative = value < 0;
        String result;

        if (is_negative)
            value = -value;

        while (value > 0)
        {
            auto character = static_cast<uchar>('0' + (value % 10));
            result = character + result;
            value /= 10;
        }

        if (is_negative)
            result = TEXT("-") + result;

        return result;
    }

    PAWN_API String ToString(uint8 value)
    {
        return ToString(static_cast<uint64>(value));
    }

    PAWN_API String ToString(uint16 value)
    {
        return ToString(static_cast<uint64>(value));
    }

    PAWN_API String ToString(uint32 value)
    {
        return ToString(static_cast<uint64>(value));
    }

    PAWN_API String ToString(uint64 value)
    {
        if (value == 0) return TEXT("0");

        String result;
        while (value > 0)
        {
            result = static_cast<uchar>('0' + (value % 10)) + result;
            value /= 10;
        }
        return result;
    }

    PAWN_API String ToString(float32 value)
    {
        return ToString(static_cast<float64>(value));
    }

    PAWN_API String ToString(float64 value)
    {
        if (value == 0.0) return TEXT("0.0");

        bool is_negative = value < 0;
        if (is_negative) value = -value;

        int64 int_part = static_cast<int64>(value);
        float64 frac_part = value - int_part;

        String result = ToString(int_part);
        result += TEXT(".");

        const int precision = 6;
        for (int i = 0; i < precision && frac_part > 0; i++)
        {
            frac_part *= 10;
            int digit = static_cast<int>(frac_part);
            result += static_cast<uchar>('0' + digit);
            frac_part -= digit;
        }

        while (result.GetSize() > 1 && result[result.GetSize() - 1] == TEXT('0'))
            result.PopBack();

        if (result[result.GetSize() - 1] == TEXT('.'))
            result.PopBack();

        if (is_negative)
            result = TEXT("-") + result;

        return result;
    }

    PAWN_API String ToString(bool value)
    {
        return value ? TEXT("true") : TEXT("false");
    }

    PAWN_API String ToString(const String& value)
    {
        return value;
    }

    PAWN_API String ToString(const uchar& value)
    {
        return String(&value, 1);
    }

    PAWN_API String operator+(const uchar* str1, const String& str2)
    {
        String result(str1);
        result += str2.GetString();
        return result;
    }

    PAWN_API String operator+(const uchar& str1, const String& str2)
    {
        String result = ToString(str1);
        result += str2.GetString();
        return result;
    }

}