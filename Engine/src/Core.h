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

#ifdef _DEBUG
	#define PE_ASSERT(x, ...) if (!x) PE_FATAL(__VA_ARGS__)
	#define PE_CORE_ASSERT(x, ...) if (!x) PE_CORE	_FATAL(__VA_ARGS__)
#else
	#define PE_ASSERT(x, ...)
	#define PE_CORE_ASSERT(x, ...)
#endif