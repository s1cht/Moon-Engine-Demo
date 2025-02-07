#include "pch.h"
#include "String.h"
#include "Core/Core.h"


const uchar* operator""_uchar(const char* str, SIZE_T strSize)
{
	uchar* newStr = (uchar*)malloc((strSize + 1) * sizeof(uchar));
	if (!newStr)
		return nullptr;

	memcpy((void*)newStr, (void*)str, strSize);
	newStr[strSize] = '\0';

	return newStr;
}