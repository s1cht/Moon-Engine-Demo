#pragma once

#include "Core/Types.hpp"
#include "Core/Macros.hpp"

#include <cstring>
#include <initializer_list>

#define CORE_API
#define PAWN_API

#if defined(_MSC_VER)
	#include "Core/Platform/Windows/MSVCErrors.hpp"
#endif

#if defined(PLATFORM_WINDOWS)
	#define _CRT_SECURE_NO_WARNINGS
	#define SPDLOG_WCHAR_TO_UTF8_SUPPORT

	#undef CORE_API
	#undef PAWN_API

	#ifdef PAWN_LIBRARY_BUILD
		#define PAWN_API DLLEXPORT
	#else	
		#define PAWN_API DLLIMPORT
	#endif 

	#ifdef PAWN_CORE_LIBRARY_BUILD
		#define CORE_API DLLEXPORT
		#define XXH_EXPORT
	#else	
		#define CORE_API DLLIMPORT
		#define XXH_IMPORT
	#endif 

#elif defined(PLATFORM_LINUX)

#else
	#error "Pawn Engine currently supports only Windows!"
#endif