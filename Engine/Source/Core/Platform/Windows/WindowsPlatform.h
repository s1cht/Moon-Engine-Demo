#pragma once

#include "Core/Platform/Base/BaseTypes.h"

// Windows platform types redefinition

#define UTF8_ENCODING

struct WindowsPlatformTypes : BaseTypes
{
#ifdef _WIN64
	typedef unsigned __int64 size;
	typedef __int64 ssize;
#else
	typedef unsigned long usize;
	typedef long size;
#endif

#ifdef UTF8_ENCODING
	typedef char8 uchar;
#endif

};

typedef WindowsPlatformTypes PlatformTypes;


#define DLLIMPORT __declspec(dllimport)
#define DLLEXPORT __declspec(dllexport)