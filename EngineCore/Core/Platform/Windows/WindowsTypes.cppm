module;
export module Pawn.Core.Types.Windows;
import Pawn.Core.Types.Base;

// Windows platform types redefinition

struct WindowsPlatformTypes : BaseTypes
{
	#ifdef _WIN64
		typedef unsigned __int64 SIZE_T;
		typedef __int64 SSIZE_T;
	#else
		typedef unsigned long SIZE_T;
		typedef long SSIZE_T;
	#endif

};

export typedef WindowsPlatformTypes PlatformTypes;