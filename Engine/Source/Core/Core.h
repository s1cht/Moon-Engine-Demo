#pragma once

#include "CoreTypes.h"

#ifdef PLATFORM_WINDOWS
	#define SPDLOG_WCHAR_TO_UTF8_SUPPORT

	#ifdef PAWN_LIBRARY_BUILD
		#define PAWN_API DLLEXPORT
	#else	
		#define PAWN_API DLLIMPORT
	#endif 
#else
	#error Pawn Engine currently supports only Windows!
#endif