#pragma once

#include "Core/Platform/Platform.h"

/*							Characters								*/

// Ansi character
typedef PlatformTypes::ansichar					ansichar;

// Wide character
typedef PlatformTypes::wchar					wchar;

// Base character (not unsigned char)
typedef PlatformTypes::uchar					uchar;

// UTF-8 char
typedef PlatformTypes::char8					char8;

// UTF-16 char
typedef PlatformTypes::char16					char16;

// UTF-32 char
typedef PlatformTypes::char32					char32;

/*							Integers								*/

// 8-bit integer
typedef PlatformTypes::int8						int8;

// 16-bit integer
typedef PlatformTypes::int16					int16;

// 32-bit integer
typedef PlatformTypes::int32					int32;

// 64-bit integer
typedef PlatformTypes::int64					int64;


// 8-bit unsigned integer
typedef PlatformTypes::uint8					uint8;

// 16-bit unsigned integer
typedef PlatformTypes::uint16					uint16;

// 32-bit unsigned integer
typedef PlatformTypes::uint32					uint32;

// 64-bit unsigned integer
typedef PlatformTypes::uint64					uint64;


// Signed IntPtr
typedef PlatformTypes::intptr					intptr;

// Unsigned IntPtr
typedef PlatformTypes::uintptr					uintptr;

// Signed size_t
typedef PlatformTypes::SSIZE_T					SSIZE_T;

// Unsigned size_t
typedef PlatformTypes::SIZE_T					SIZE_T;

/*					Numbers with floating point						*/

// 32-bit float
typedef PlatformTypes::float32					float32;

// 64-bit float
typedef PlatformTypes::float64					float64;