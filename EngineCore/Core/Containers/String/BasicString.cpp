#include "BasicString.hpp"

namespace Pawn::Core::Containers
{
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

    AnsiString ToAnsiString(float32 value)
    {
        return ToAnsiString(static_cast<float64>(value));
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

    WideString ToWideString(int8 value)
    {
        return ToWideString(static_cast<int64>(value));
    }

    WideString ToWideString(int16 value)
    {
        return ToWideString(static_cast<int64>(value));
    }

    WideString ToWideString(int32 value)
    {
        return ToWideString(static_cast<int64>(value));
    }

    WideString ToWideString(uint8 value)
    {
        return ToWideString(static_cast<uint64>(value));
    }

    WideString ToWideString(uint16 value)
    {
        return ToWideString(static_cast<uint64>(value));
    }

    WideString ToWideString(uint32 value)
    {
        return ToWideString(static_cast<uint64>(value));
    }

    WideString ToWideString(float32 value)
    {
        return ToWideString(static_cast<float64>(value));
    }

    WideString ToWideString(bool value)
    {
        return value ? L"true" : L"false";
    }

    WideString ToWideString(const WideString& value)
    {
        return value;
    }

    WideString ToWideString(const wchar& value)
    {
        return WideString(&value, 1);
    }

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

    String ToString(uint8 value)
    {
        return ToWideString(static_cast<uint64>(value));
    }

    String ToString(uint16 value)
    {
        return ToWideString(static_cast<uint64>(value));
    }

    String ToString(uint32 value)
    {
        return ToWideString(static_cast<uint64>(value));
    }

    String ToString(float32 value)
    {
        return ToWideString(static_cast<float64>(value));
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

    //
    // Conversion implementations
    //

    AnsiString ToAnsiString(int64 value)
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

    int64 AnsiStringToInt(const AnsiString& str, SIZE_T* pos, int8 base)
    {
        int64 result;
        ansichar* endPtr;
        const ansichar* strSt = str.GetString();

    	result = strtoll(strSt, &endPtr, base);

        if (pos != nullptr)
            *pos = static_cast<SIZE_T>(endPtr - strSt);

        return result;
    }

    uint64 AnsiStringToUint(const AnsiString& str, SIZE_T* pos, int8 base)
    {
        uint64 result;
        ansichar* endPtr;
        const ansichar* strSt = str.GetString();

    	result = strtoull(strSt, &endPtr, base);

        if (pos != nullptr)
            *pos = static_cast<SIZE_T>(endPtr - strSt);

        return result;
    }

    float64 AnsiStringToFloat(const AnsiString& str, SIZE_T* pos)
    {
        float64 result;
        ansichar* endPtr;
        const ansichar* strSt = str.GetString();

        result = strtod(strSt, &endPtr);

        if (pos != nullptr)
            *pos = static_cast<SIZE_T>(endPtr - strSt);

        return result;
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

    WideString ToWideString(int64 value)
    {
        if (value == 0) return L"0";

        bool is_negative = value < 0;
        WideString result;

        if (is_negative)
            value = -value;

        while (value > 0)
        {
            auto character = static_cast<wchar>(L'0' + (value % 10));
            result = character + result;
            value /= 10;
        }

        if (is_negative)
            result = L"-" + result;

        return result;
    }

    WideString ToWideString(uint64 value)
    {
        if (value == 0) return L"0";

        WideString result;
        while (value > 0)
        {
            result = static_cast<wchar>(L'0' + (value % 10)) + result;
            value /= 10;
        }
        return result;
    }

    WideString ToWideString(float64 value)
    {
        if (value == 0.0) return L"0.0";

        bool is_negative = value < 0;
        if (is_negative) value = -value;

        int64 int_part = static_cast<int64>(value);
        float64 frac_part = value - int_part;

        WideString result = ToWideString(int_part);
        result += L".";

        const int precision = 6;
        for (int i = 0; i < precision && frac_part > 0; i++)
        {
            frac_part *= 10;
            int digit = static_cast<int>(frac_part);
            result += static_cast<wchar>(L'0' + digit);
            frac_part -= digit;
        }

        while (result.GetSize() > 1 && result[result.GetSize() - 1] == L'0')
            result.PopBack();

        if (result[result.GetSize() - 1] == L'.')
            result.PopBack();

        if (is_negative)
            result = L"-" + result;

        return result;
    }

    int64 WideStringToInt(const WideString& str, SIZE_T* pos, int8 base)
    {
        int64 result;
        wchar* endPtr;
        const wchar* strSt = str.GetString();

        result = wcstoll(strSt, &endPtr, base);

        if (pos != nullptr)
            *pos = static_cast<SIZE_T>(endPtr - strSt);

        return result;
    }

    uint64 WideStringToUint(const WideString& str, SIZE_T* pos, int8 base)
    {
        uint64 result;
        wchar* endPtr;
        const wchar* strSt = str.GetString();

        result = wcstoull(strSt, &endPtr, base);

        if (pos != nullptr)
            *pos = static_cast<SIZE_T>(endPtr - strSt);

        return result;
    }

    float64 WideStringToFloat(const WideString& str, SIZE_T* pos)
    {
        float64 result;
        wchar* endPtr;
        const wchar* strSt = str.GetString();

        result = wcstod(strSt, &endPtr);

        if (pos != nullptr)
            *pos = static_cast<SIZE_T>(endPtr - strSt);

        return result;
    }

    WideString operator+(const wchar* str1, const WideString& str2)
    {
        WideString result(str1);
        result += str2.GetString();
        return result;
    }

    WideString operator+(const wchar& str1, const WideString& str2)
    {
        WideString result = ToWideString(str1);
        result += str2.GetString();
        return result;
    }

    inline String ToString(int64 value)
    {
        return sizeof(uchar) == 1 ? AnsiStringToString(ToAnsiString(value)): WideStringToString(ToWideString(value));
    }

    inline String ToString(uint64 value)
    {
        return sizeof(uchar) == 1 ? AnsiStringToString(ToAnsiString(value)): WideStringToString(ToWideString(value));
    }

    inline String ToString(float64 value)
    {
        return sizeof(uchar) == 1 ? AnsiStringToString(ToAnsiString(value)): WideStringToString(ToWideString(value));
    }

    int64 StringToInt(const String& str, SIZE_T* pos, int8 base)
    {
        return sizeof(uchar) == 1 ? AnsiStringToInt(StringToAnsiString(str), pos, base) : WideStringToInt(StringToWideString(str), pos, base);
    }

    uint64 StringToUint(const String& str, SIZE_T* pos, int8 base)
    {
        return sizeof(uchar) == 1 ? AnsiStringToUint(StringToAnsiString(str), pos, base) : WideStringToUint(StringToWideString(str), pos, base);
    }

    float64 StringToFloat(const String& str, SIZE_T* pos)
    {
        return sizeof(uchar) == 1 ? AnsiStringToFloat(StringToAnsiString(str), pos) : WideStringToFloat(StringToWideString(str), pos);
    }
		
    inline AnsiString WideStringToAnsiString(const WideString& str)
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

    WideString AnsiStringToWideString(const AnsiString& str)
    {
        WideString result;
        wchar* resultStr;
        SIZE_T strSize;

        strSize = str.GetSize();
        resultStr = new wchar[strSize];

        mbstowcs(resultStr, str.GetString(), strSize);

        result = WideString(resultStr, strSize);

        delete resultStr;

        return result;
    }

}