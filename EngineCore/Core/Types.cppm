module;
export module Pawn.Core.Types;

#include "Platform/Platform.h"

#if defined(PLATFORM_WINDOWS)
	import Pawn.Core.Types.Windows;
#elif defined(PLATFORM_LINUX)
	import Pawn.Core.Types.Linux;
#else
	import Pawn.Core.Types.Base;
	struct PlatformTypes : BaseTypes
	{
		typedef uchar ansichar;
	};
#endif 

/*							Characters								*/

// Ansi character
export typedef PlatformTypes::ansichar						ansichar;

// Wide character
export typedef PlatformTypes::wchar						wchar;

// Base character (not unsigned char)
export typedef PlatformTypes::uchar						uchar;

// UTF-8 char
export typedef PlatformTypes::char8						char8;

// UTF-16 char
export typedef PlatformTypes::char16						char16;

// UTF-32 char
export typedef PlatformTypes::char32						char32;

/*							Integers								*/

// 8-bit integer
export typedef PlatformTypes::int8							int8;

// 16-bit integer
export typedef PlatformTypes::int16						int16;

// 32-bit integer
export typedef PlatformTypes::int32						int32;

// 64-bit integer
export typedef PlatformTypes::int64						int64;


// 8-bit unsigned integer
export typedef PlatformTypes::uint8						uint8;

// 16-bit unsigned integer
export typedef PlatformTypes::uint16						uint16;

// 32-bit unsigned integer
export typedef PlatformTypes::uint32						uint32;

// 64-bit unsigned integer
export typedef PlatformTypes::uint64						uint64;

// Signed IntPtr
export typedef PlatformTypes::intptr						intptr;

// Unsigned IntPtr
export typedef PlatformTypes::uintptr						uintptr;

// Signed size_t
export typedef PlatformTypes::SSIZE_T						SSIZE_T;

// Unsigned size_t
export typedef PlatformTypes::SIZE_T						SIZE_T;

/*					Numbers with floating point						*/

// 32-bit float
export typedef PlatformTypes::float32						float32;

// 64-bit float
export typedef PlatformTypes::float64						float64;