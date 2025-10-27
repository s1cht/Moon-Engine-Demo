#pragma once

#include <type_traits>

struct BaseTypes 
{
	/*
										-INTEGERS-
				
					The structure of defining integer types is as follows:
				
									1. 8-bit u/s int
									2. 16-bit u/s int
									3. 32-bit u/s int
									4. 64-bit u/s int
				
								u/s - unsigned / signed
	*/
	
	// SIGNED
	typedef signed char					int8;			// Signed 1.	(Check "INTEGERS" reference)
	typedef signed short				int16;			// Signed 2.	(Check "INTEGERS" reference)
	typedef signed int					int32;			// Signed 3.	(Check "INTEGERS" reference)
	typedef signed long long			int64;			// Signed 4.	(Check "INTEGERS" reference)

	// UNSIGNED
	typedef unsigned char				uint8;			// Unsigned 1.	(Check "INTEGERS" reference)
	typedef unsigned short				uint16;			// Unsigned 2.	(Check "INTEGERS" reference)
	typedef unsigned int				uint32;			// Unsigned 3.	(Check "INTEGERS" reference)
	typedef unsigned long long			uint64;			// Unsigned 4.	(Check "INTEGERS" reference)

	/*
									-NUMBER WITH FLOATING POINT-

				The structure of defining number with floating point types is as follows:

								        1. 32-bit float
								        2. 64-bit float
	*/

	typedef float						float32;		// 1.
	typedef double						float64;		// 2.


	/*
										-CHARACTERS-

					The structure of defining character types is as follows:

									1. 8-bit character
									2. 16-bit character
									3. 32-bit character
									
									4. ASCII character
									5. Wide character
	*/

	typedef char8_t						char8;			// 1. (Check "CHARACTERS" reference)
	typedef uint16						char16;			// 2. (Check "CHARACTERS" reference)
	typedef uint32						char32;			// 3. (Check "CHARACTERS" reference)
	
	typedef char						asciichar;		// 4. (Check "CHARACTERS" reference)
	typedef wchar_t						wchar;			// 5. (Check "CHARACTERS" reference)

	/* 
										-INT POINTER-
				
					The structure of defining int pointer types is as follows:
				
										1. Signed
										2. Unsigned

	*/

	typedef std::conditional_t < sizeof(void*) == 4, int32, int64>				intptr;		// 1. (Check "INT POINTER" reference)
	typedef std::conditional_t < sizeof(void*) == 4, uint32, uint64>			uintptr;	// 2. (Check "INT POINTER" reference)

	// Unsigned size_t
	typedef uintptr SIZE_T;
	// Signed size_t
	typedef intptr SSIZE_T;

};