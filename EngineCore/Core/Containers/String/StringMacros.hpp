#pragma once

#include "Core/Platform/Platform.hpp"

#undef TEXT

#ifdef PLATFORM_WINDOWS
	#define TEXT_JOIN(x) L ## x
	#define TEXT_HELPER(x) TEXT_JOIN(x)
	#define TEXT(x) TEXT_HELPER(x)
#else
	#define TEXT(x) x
#endif


#define ME_NAMED_VARIABLE_TOSTRING(type, value, name) TEXT(" [") TEXT(#type) TEXT("] ") name TEXT(" = ") + ME::Core::Containers::ToString(value) + TEXT(";")