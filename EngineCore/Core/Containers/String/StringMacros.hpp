#pragma once

#include "Core/Platform/Platform.hpp"

#undef TEXT

#ifdef PLATFORM_WINDOWS
	#define TEXT(x) L ## x
#else
	#define TEXT(x) x
#endif

#define PE_NAMED_VARIABLE_TOSTRING(type, value, name) TEXT(" [") TEXT(#type) TEXT("] ") name TEXT(" = ") + Pawn::Core::Containers::ToString(value) + TEXT(";")