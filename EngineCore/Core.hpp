#pragma once
#include "Core/Types.hpp"
#include "Core/Macros.hpp"
#include <cstring>
#include <concepts>
#include <limits>
#include <initializer_list>

#if defined(_MSC_VER)
	#include "Core/Platform/Windows/MSVCErrors.hpp"
#endif

#define MEAPI
#define COREAPI

#if defined(PLATFORM_WINDOWS)
#	define _CRT_SECURE_NO_WARNINGS
#	define SPDLOG_WCHAR_TO_UTF8_SUPPORT
#
#	undef COREAPI
#	undef MEAPI
#
#	ifdef MOON_LIBRARY_BUILD
#		define MEAPI DLLEXPORT
#	else	
#		define MEAPI DLLIMPORT
#	endif 
#
#	ifdef MOON_CORE_LIBRARY_BUILD
#		define COREAPI DLLEXPORT
#	else	
#		define COREAPI DLLIMPORT
#	endif 
#
#elif defined(PLATFORM_LINUX)
#
#	//Do nothing this moment
#
#else#
#	error "Moon Engine currently supports only Windows!"
#endif