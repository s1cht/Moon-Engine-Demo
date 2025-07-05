#pragma once

#include "Core/Types.hpp"
#include "Core/Macros.hpp"
#include <cstring>
#include <initializer_list>

#if defined(_MSC_VER)
	#include "Core/Platform/Windows/MSVCErrors.hpp"
#endif

#define PAWN_API
#define CORE_API

#if defined(PLATFORM_WINDOWS)
#	define _CRT_SECURE_NO_WARNINGS
#	define SPDLOG_WCHAR_TO_UTF8_SUPPORT
#
#	undef CORE_API
#	undef PAWN_API
#
#	ifdef PAWN_LIBRARY_BUILD
#		define PAWN_API DLLEXPORT
#	else	
#		define PAWN_API DLLIMPORT
#	endif 
#
#	ifdef PAWN_CORE_LIBRARY_BUILD
#		define CORE_API DLLEXPORT
#	else	
#		define CORE_API DLLIMPORT
#		// for xxHash
//#		define XXH_IMPORT 1
#	endif 
#
#elif defined(PLATFORM_LINUX)
#
#	//Do nothing this moment
#
#else#
#	error "Pawn Engine currently supports only Windows!"
#endif