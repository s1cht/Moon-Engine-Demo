#pragma once

#include "Core/Platform/Platform.hpp"

#undef TEXT

#define TEXT_JOINW(x) L ## x
#define TEXT_HELPERW(x) TEXT_JOINW(x)

#define TEXT_JOIN(x) u8 ## x
#define TEXT_HELPER(x) TEXT_JOIN(x)

#ifdef PLATFORM_WINDOWS
	#define TEXT(x) TEXT_HELPER(x)
	#define TEXTW(x) TEXT_HELPERW(x)
#else
	#define TEXT(x) x
#endif


#define ME_NAMED_VARIABLE_TOSTRING(type, value, name) TEXT(" [") TEXT(#type) TEXT("] ") name TEXT(" = ") + ME::Core::ToString(value) + TEXT(";")