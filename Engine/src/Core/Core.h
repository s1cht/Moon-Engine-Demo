#pragma once

#include "Types.h"
#include "Misc/CoreAssertion.h"

#ifdef _WIN32
	#ifdef PAWN_LIBRARY_BUILD
		#define ENGINE_API __declspec(dllexport)
	#else	
		#define ENGINE_API __declspec(dllimport)
	#endif 
#else
	#error Pawn Engine currently supports only Windows!
#endif