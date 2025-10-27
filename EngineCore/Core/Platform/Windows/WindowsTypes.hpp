#pragma once

#include "Core/Platform/Base/BaseTypes.hpp"

// Windows platform types redefinition

struct WindowsPlatformTypes : BaseTypes
{
	#ifdef _WIN64
		typedef unsigned __int64 SIZE_T;
		typedef __int64 SSIZE_T;
	#else
		typedef unsigned long SIZE_T;
		typedef long SSIZE_T;
	#endif

};

typedef WindowsPlatformTypes PlatformTypes;