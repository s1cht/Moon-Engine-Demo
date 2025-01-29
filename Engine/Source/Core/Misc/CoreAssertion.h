#pragma once
#include "pch.h"

#ifdef _DEBUG
	#define PE_ASSERT(x, ...) if (!x) PE_FATAL(__VA_ARGS__)
	#define PE_CORE_ASSERT(x, ...) if (!x) PE_CORE_FATAL(__VA_ARGS__)
#else
	#define PE_ASSERT(x, ...)
	#define PE_CORE_ASSERT(x, ...)
#endif