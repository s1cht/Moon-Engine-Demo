#pragma once

#if defined(__aarch64__) || defined(_M_AMD64) || defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
	#define ARCHITECTURE_X64
#else
	#error "Unsupported processor architecture"
#endif

#if defined(_WIN64)
	#define PLATFORM_WINDOWS
	#include "Windows/WindowsMacros.h"
#elif defined(__linux__)
	#define PLATFORM_LINUX
#else
	#error "Unsupported system"
#endif

