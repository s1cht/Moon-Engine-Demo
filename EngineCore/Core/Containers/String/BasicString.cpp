#include "BasicString.hpp"

namespace ME::Core
{
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

    String ToString(float32 value)
    {
        return ToString(static_cast<float64>(value));
    }

    String ToString(bool value)
    {
        return value ? TEXT("true") : TEXT("false");
    }

    String ToString(const char8& value)
    {
        return String(&value, 1);
    }

    String ToString(const String& value)
    {
        return value;
    }

    //
    // Conversion implementations
    //

    String ToString(int64 value)
    {
        if (value == 0) return '0';

        bool is_negative = value < 0;
        String result;

        if (is_negative)
            value = -value;

        while (value > 0)
        {
            auto character = static_cast<char8>('0' + (value % 10));
            result = character + result;
            value /= 10;
        }

        if (is_negative)
            result = "-" + result;

        return result;
    }

    String ToString(uint64 value)
    {
        if (value == 0) return "0";

        String result;
        while (value > 0)
        {
            result = static_cast<char8>('0' + (value % 10)) + result;
            value /= 10;
        }
        return result;
    }

    String ToString(float64 value)
    {
        if (value == 0.0) return "0.0";

        bool is_negative = value < 0;
        if (is_negative) value = -value;

        int64 int_part = static_cast<int64>(value);
        float64 frac_part = value - static_cast<float64>(int_part);

        String result = ToString(int_part);
        result += ".";

        constexpr int precision = 6;
        for (int i = 0; i < precision && frac_part > 0; i++)
        {
            frac_part *= 10;
            int digit = static_cast<int>(frac_part);
            result += static_cast<char8>('0' + digit);
            frac_part -= digit;
        }

        while (result.Size() > 1 && result[result.Size() - 1] == TEXT('0'))
            result.PopBack();

        if (result[result.Size() - 1] == TEXT('.'))
            result.PopBack();

        if (is_negative)
            result = "-" + result;

        return result;
    }

    int64 StringToInt(const String& str, SIZE_T* pos, int8 base)
    {
        asciichar* endPtr;
        const asciichar* strSt = reinterpret_cast<const asciichar*>(str.String());

    	int64 result = strtoll(strSt, &endPtr, base);

        if (pos != nullptr)
            *pos = static_cast<SIZE_T>(endPtr - strSt);

        return result;
    }

    uint64 StringToUint(const String& str, SIZE_T* pos, int8 base)
    {
        asciichar* endPtr;
        const asciichar* strSt = reinterpret_cast<const asciichar*>(str.String());

    	uint64 result = strtoull(strSt, &endPtr, base);

        if (pos != nullptr)
            *pos = static_cast<SIZE_T>(endPtr - strSt);

        return result;
    }

    float64 StringToFloat(const String& str, SIZE_T* pos)
    {
        asciichar* endPtr;
        const asciichar* strSt = reinterpret_cast<const asciichar*>(str.String());

        float64 result = strtod(strSt, &endPtr);

        if (pos != nullptr)
            *pos = static_cast<SIZE_T>(endPtr - strSt);

        return result;
    }

    String operator+(const char8* str1, const String& str2)
    {
        String result(str1);
        result += str2.String();
        return result;
    }

    String operator+(const char8& str1, const String& str2)
    {
        String result = ToString(str1);
        result += str2.String();
        return result;
    }

    String operator+(const asciichar* str1, const String& str2)
    {
        String result(str1);
        result += str2.String();
        return result;
    }

    String operator+(const asciichar& str1, const String& str2)
    {
        String result = ToString(str1);
        result += str2.String();
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
        float64 frac_part = value - static_cast<float64>(int_part);

        WideString result = ToWideString(int_part);
        result += L".";

        constexpr int precision = 6;
        for (int i = 0; i < precision && frac_part > 0; i++)
        {
            frac_part *= 10;
            int digit = static_cast<int>(frac_part);
            result += static_cast<wchar>(L'0' + digit);
            frac_part -= digit;
        }

        while (result.Size() > 1 && result[result.Size() - 1] == L'0')
            result.PopBack();

        if (result[result.Size() - 1] == L'.')
            result.PopBack();

        if (is_negative)
            result = L"-" + result;

        return result;
    }

    int64 WideStringToInt(const WideString& str, SIZE_T* pos, int8 base)
    {
        wchar* endPtr;
        const wchar* strSt = str.String();

        int64 result = wcstoll(strSt, &endPtr, base);

        if (pos != nullptr)
            *pos = static_cast<SIZE_T>(endPtr - strSt);

        return result;
    }

    uint64 WideStringToUint(const WideString& str, SIZE_T* pos, int8 base)
    {
        wchar* endPtr;
        const wchar* strSt = str.String();

        uint64 result = wcstoull(strSt, &endPtr, base);

        if (pos != nullptr)
            *pos = static_cast<SIZE_T>(endPtr - strSt);

        return result;
    }

    float64 WideStringToFloat(const WideString& str, SIZE_T* pos)
    {
        wchar* endPtr;
        const wchar* strSt = str.String();

        float64 result = wcstod(strSt, &endPtr);

        if (pos != nullptr)
            *pos = static_cast<SIZE_T>(endPtr - strSt);

        return result;
    }

    WideString operator+(const wchar* str1, const WideString& str2)
    {
        WideString result(str1);
        result += str2.String();
        return result;
    }

    WideString operator+(const wchar& str1, const WideString& str2)
    {
        WideString result = ToWideString(str1);
        result += str2.String();
        return result;
    }
		
    String WideStringToString(const WideString& str)
    {
        SIZE_T num;
        SIZE_T strSize = str.Size() + 1;
        asciichar* resultStr = new asciichar[strSize];

        errno_t err = wcstombs_s(&num, resultStr, strSize, 
            str.String(), str.Size());

        if (err != 0)
        {
            delete[] resultStr;
            return "";
        }

        String result = String(resultStr, num - 1);
        delete[] resultStr;
        return result;
    }

    WideString StringToWideString(const String& str)
    {
        SIZE_T num;
        SIZE_T strSize = str.Size() + 1;
        wchar* resultStr = new wchar[strSize];
        errno_t err = mbstowcs_s(&num, resultStr, strSize, 
            reinterpret_cast<const char*>(str.String()), str.Size());

        if (err != 0)
        {
            delete[] resultStr;
            return L"";
        }

        WideString result = WideString(resultStr, num - 1);
        delete[] resultStr;
        return result;
    }

}