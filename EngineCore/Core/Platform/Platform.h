#pragma once

// STD includes
#include <atomic>

#ifdef _WIN32
	#define PLATFORM_WINDOWS
	#ifdef _WIN64
		#define ARCHITECTURE_X64
	#else	
		#define ARCHITECTURE_X86
	#endif

	#include "Windows/WindowsPlatform.h"
#else

#endif

