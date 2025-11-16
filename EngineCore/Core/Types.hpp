#pragma once
#include <atomic>
#include "Platform/Platform.hpp"

#if defined(PLATFORM_WINDOWS)
	#include "Core/Platform/Windows/WindowsTypes.hpp"
#elif defined(PLATFORM_LINUX)
	#include "Core/Platform/Unix/UnixTypes.hpp"
#else
	#include "Core/Platform/Base/BaseTypes.hpp"
#endif 

/*							Characters								*/

// Ansi character
typedef PlatformTypes::asciichar					asciichar;

// Wide character
typedef PlatformTypes::wchar						wchar;

// UTF-8 char
typedef PlatformTypes::char8						char8;

// UTF-16 char
typedef PlatformTypes::char16						char16;

// UTF-32 char
typedef PlatformTypes::char32						char32;

/*							Integers								*/

// 8-bit integer
typedef PlatformTypes::int8							int8;

// 16-bit integer
typedef PlatformTypes::int16						int16;

// 32-bit integer
typedef PlatformTypes::int32						int32;

// 64-bit integer
typedef PlatformTypes::int64						int64;


// 8-bit unsigned integer
typedef PlatformTypes::uint8						uint8;

// 16-bit unsigned integer
typedef PlatformTypes::uint16						uint16;

// 32-bit unsigned integer
typedef PlatformTypes::uint32						uint32;

// 64-bit unsigned integer
typedef PlatformTypes::uint64						uint64;

// Signed IntPtr
typedef PlatformTypes::intptr						intptr;

// Unsigned IntPtr
typedef PlatformTypes::uintptr						uintptr;

// Signed size_t
typedef PlatformTypes::SSIZE_T						SSIZE_T;

// Unsigned size_t
typedef PlatformTypes::SIZE_T						SIZE_T;

/*					Numbers with floating point						*/

// 32-bit float
typedef PlatformTypes::float32						float32;

// 64-bit float
typedef PlatformTypes::float64						float64;


/*							Atomic types							*/
namespace ME::Core
{
	template <typename T>
	using Atomic =					std::atomic<T>;

	typedef Atomic<bool>			Atomic_bool;
	typedef Atomic<asciichar>		Atomic_asciichar;
	typedef Atomic<wchar>			Atomic_wchar;
	typedef Atomic<char8>			Atomic_char8;
	typedef Atomic<char16>			Atomic_char16;
	typedef Atomic<char32>			Atomic_char32;

	typedef Atomic<int8>			Atomic_int8;
	typedef Atomic<int16>			Atomic_int16;
	typedef Atomic<int32>			Atomic_int32;
	typedef Atomic<int64>			Atomic_int64;

	typedef Atomic<int8>			Atomic_int8;
	typedef Atomic<int16>			Atomic_int16;
	typedef Atomic<int32>			Atomic_int32;
	typedef Atomic<int64>			Atomic_int64;

	typedef Atomic<uint8>			Atomic_uint8;
	typedef Atomic<uint16>			Atomic_uint16;
	typedef Atomic<uint32>			Atomic_uint32;
	typedef Atomic<uint64>			Atomic_uint64;

	typedef Atomic<SIZE_T>			Atomic_SIZE_T;
	typedef Atomic<SSIZE_T>			Atomic_SSIZE_T;
	typedef Atomic<uintptr>			Atomic_uintptr;
	typedef Atomic<intptr>			Atomic_intptr;
}