#pragma once

#include "Types.h"

#ifdef _WIN32
	#ifdef PAWN_LIBRARY_BUILD
		#define PAWN_API DLLEXPORT
	#else	
		#define PAWN_API DLLIMPORT
	#endif 
#else
	#error Pawn Engine currently supports only Windows!
#endif