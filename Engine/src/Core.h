#pragma once

#ifdef _WIN32
	#ifdef PAWN_LIBRARY_BUILD
		#define PAWN_API __declspec(dllexport)
	#else	
		#define PAWN_API __declspec(dllimport)
	#endif 
#else
	#error Pawn Engine currently supports only Windows!
#endif