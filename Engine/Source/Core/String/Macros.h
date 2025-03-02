#pragma once

#include "Core/Platform/Platform.h"

#undef TEXT

#ifdef PLATFORM_WINDOWS
	#define TEXT(x) L ## x
#else
	#define TEXT(x) x
#endif
