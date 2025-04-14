module Pawn.Core.Container.String;

#include "Core.h"
#include "Core/Misc/Assertion.h"

namespace Pawn::Core::Containers
{
    String ToString(int8 value)
    {
        return ToString(static_cast<int64>(value));
    }

    String ToString(int16 value)
    {
        return ToString(static_cast<int64>(value));
    }

    String ToString(int32 value)
    {
        return ToString(static_cast<int64>(value));
    }

    Pawn::Core::Containers::String ToString(int64 value)
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

    String ToString(uint8 value)
    {
        return ToString(static_cast<uint64>(value));
    }

    String ToString(uint16 value)
    {
        return ToString(static_cast<uint64>(value));
    }

    String ToString(uint32 value)
    {
        return ToString(static_cast<uint64>(value));
    }

    String ToString(uint64 value)
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

    String ToString(float32 value)
    {
        return ToString(static_cast<float64>(value));
    }

    String ToString(float64 value)
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

    String ToString(bool value)
    {
        return value ? TEXT("true") : TEXT("false");
    }

    String ToString(const String& value)
    {
        return value;
    }

    String ToString(const uchar& value)
    {
        return String(&value, 1);
    }

    String operator+(const uchar* str1, const String& str2)
    {
        String result(str1);
        result += str2.GetString();
        return result;
    }

    String operator+(const uchar& str1, const String& str2)
    {
        String result = ToString(str1);
        result += str2.GetString();
        return result;
    }

    AnsiString ToAnsiString(int8 value)
    {
        return ToAnsiString(static_cast<int64>(value));
    }

    AnsiString ToAnsiString(int16 value)
    {
        return ToAnsiString(static_cast<int64>(value));
    }

    AnsiString ToAnsiString(int32 value)
    {
        return ToAnsiString(static_cast<int64>(value));
    }

    AnsiString ToAnsiString(int64 value)
    {
        {
            if (value == 0) return "0";

            bool is_negative = value < 0;
            AnsiString result;

            if (is_negative)
                value = -value;

            while (value > 0)
            {
                auto character = static_cast<ansichar>('0' + (value % 10));
                result = character + result;
                value /= 10;
            }

            if (is_negative)
                result = "-" + result;

            return result;
        }
    }

    AnsiString ToAnsiString(uint8 value)
    {
        return ToAnsiString(static_cast<uint64>(value));
    }

    AnsiString ToAnsiString(uint16 value)
    {
        return ToAnsiString(static_cast<uint64>(value));
    }

    AnsiString ToAnsiString(uint32 value)
    {
        return ToAnsiString(static_cast<uint64>(value));
    }

    AnsiString ToAnsiString(uint64 value)
    {
        if (value == 0) return "0";

        AnsiString result;
        while (value > 0)
        {
            result = static_cast<ansichar>('0' + (value % 10)) + result;
            value /= 10;
        }
        return result;
    }

    AnsiString ToAnsiString(float32 value)
    {
        return ToAnsiString(static_cast<float64>(value));
    }

    AnsiString ToAnsiString(float64 value)
    {
        if (value == 0.0) return "0.0";

        bool is_negative = value < 0;
        if (is_negative) value = -value;

        int64 int_part = static_cast<int64>(value);
        float64 frac_part = value - int_part;

        AnsiString result = ToAnsiString(int_part);
        result += ".";

        const int precision = 6;
        for (int i = 0; i < precision && frac_part > 0; i++)
        {
            frac_part *= 10;
            int digit = static_cast<int>(frac_part);
            result += static_cast<ansichar>('0' + digit);
            frac_part -= digit;
        }

        while (result.GetSize() > 1 && result[result.GetSize() - 1] == TEXT('0'))
            result.PopBack();

        if (result[result.GetSize() - 1] == TEXT('.'))
            result.PopBack();

        if (is_negative)
            result = "-" + result;

        return result;
    }

    AnsiString ToAnsiString(bool value)
    {
        return value ? "true" : "false";
    }

    AnsiString ToAnsiString(const AnsiString& value)
    {
        return value;
    }

    AnsiString ToAnsiString(const ansichar& value)
    {
        return AnsiString(&value, 1);
    }

    int64 StringToInt(const String& str, SIZE_T* pos, int8 base)
    {
        int64 result;
        uchar* endPtr;
        const uchar* strSt = str.GetString();

        if (sizeof(uchar) == 2)
            result = wcstoll(strSt, &endPtr, base);
        else if (sizeof(uchar) == 1)
            result = wcstoll(strSt, &endPtr, base);
        else
            PE_ASSERT(false, TEXT("Currently 8-bit & 16-bit strings are supported!"));

        if (pos != nullptr)
        {
            *pos = static_cast<SIZE_T>(endPtr - strSt);
        }

        return result;
    }

    uint64 StringToUint(const String& str, SIZE_T* pos, int8 base)
    {
        uint64 result;
        uchar* endPtr;
        const uchar* strSt = str.GetString();

        if (sizeof(uchar) == 2)
            result = wcstoull(strSt, &endPtr, base);
        else if (sizeof(uchar) == 1)
            result = wcstoull(strSt, &endPtr, base);
        else
            PE_ASSERT(false, TEXT("Currently 8-bit & 16-bit strings are supported!"));

        if (pos != nullptr)
        {
            *pos = static_cast<SIZE_T>(endPtr - strSt);
        }

        return result;
    }

    float64 StringToFloat(const String& str, SIZE_T* pos)
    {
        float64 result;
        uchar* endPtr;
        const uchar* strSt = str.GetString();

        if (sizeof(uchar) == 2)
            result = wcstod(strSt, &endPtr);
        else if (sizeof(uchar) == 1)
            result = wcstod(strSt, &endPtr);
        else
            PE_ASSERT(false, TEXT("Currently 8-bit & 16-bit strings are supported!"));

        if (pos != nullptr)
        {
            *pos = static_cast<SIZE_T>(endPtr - strSt);
        }

        return result;
    }

    AnsiString StringToAnsiString(const String& str)
    {
        if (sizeof(uchar) == 1)
        {
            return AnsiString(reinterpret_cast<const ansichar*>(str.GetString()), str.GetSize());
        }
        else if (sizeof(uchar) == 2)
        {
            AnsiString result;
            ansichar* resultStr;
            SIZE_T strSize;

            strSize = str.GetSize();
            resultStr = new ansichar[strSize];

            wcstombs(resultStr, str.GetString(), strSize);

            result = AnsiString(resultStr, strSize);

            delete resultStr;

            return result;
        }

        return AnsiString("", 0);
    }

    CORE_API String AnsiStringToString(const AnsiString& str)
    {
		if (sizeof(uchar) == 1)
		{
			return String(reinterpret_cast<const uchar*>(str.GetString()), str.GetSize());
		}
		else if (sizeof(uchar) == 2)
		{
            String result;
			uchar* resultStr;
			SIZE_T strSize;

			strSize = str.GetSize();
			resultStr = new uchar[strSize];

			mbstowcs(resultStr, str.GetString(), strSize);

			result = String(resultStr, strSize);

			delete resultStr;

			return result;
		}

		return String(TEXT(""), 0);
    }

    AnsiString operator+(const ansichar* str1, const AnsiString& str2)
    {
        AnsiString result(str1);
		result += str2.GetString();
		return result;
    }

    AnsiString operator+(const ansichar& str1, const AnsiString& str2)
    {
        AnsiString result = ToAnsiString(str1);
		result += str2.GetString();
		return result;
    }

}