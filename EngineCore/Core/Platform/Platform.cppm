module;

#include "Platform.h"

export module Pawn.Platform;

#if defined(PLATFORM_WINDOWS)
	export import Pawn.Windows;
#endif