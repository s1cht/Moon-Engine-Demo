#pragma once

#include "Core/CoreTypes.h"
#include "Core/Bitwise.h"

#ifdef PLATFORM_WINDOWS

	#define _CRT_SECURE_NO_WARNINGS
	#define SPDLOG_WCHAR_TO_UTF8_SUPPORT

	#ifdef PAWN_LIBRARY_BUILD
		#define PAWN_API DLLEXPORT
	#else	
		#define PAWN_API DLLIMPORT
	#endif 

	#ifdef PAWN_CORE_LIBRARY_BUILD
		#define CORE_API DLLEXPORT
	#else	
		#define CORE_API DLLIMPORT
	#endif 
	
#else
	#error Pawn Engine currently supports only Windows!
#endif