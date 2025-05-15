#pragma once

#include "Core/Types.hpp"
#include "Core/Macros.hpp"
#include "Core/Memory/Allocator.hpp"
#include "Core/Memory/PawnMemory.hpp"
#include "Core/Containers/Algorithm.hpp"
#include "Core/Containers/Array.hpp"
#include "Core/Containers/String/String.hpp"
#include "Core/Containers/Tables/UnorderedMap.hpp"
#include "Core/Platform/Platform.hpp"
#include "Core/Misc/Pair.hpp"
#include "Core/Misc/Time.hpp"
#include "Core/Layer/Layer.hpp"
#include "Core/Layer/LayerStack.hpp"
#include "Core/Events/Event.hpp"
#include "Core/Math/Math.hpp"

#include <cstring>

#define CORE_API
#define PAWN_API

#if defined(_MSC_VER)
	#include "Core/Platform/Windows/MSVCErrors.hpp"
#endif

#if defined(PLATFORM_WINDOWS)
	#define _CRT_SECURE_NO_WARNINGS
	#define SPDLOG_WCHAR_TO_UTF8_SUPPORT

	#undef CORE_API
	#undef PAWN_API

	#ifdef PAWN_LIBRARY_BUILD
		#define PAWN_API DLLEXPORT
	#else	
		#define PAWN_API DLLIMPORT
	#endif 

	#ifdef PAWN_CORE_LIBRARY_BUILD
		#define CORE_API DLLEXPORT
		#define XXH_EXPORT
	#else	
		#define CORE_API DLLIMPORT
		#define XXH_IMPORT
	#endif 

#else
	#error "Pawn Engine currently supports only Windows!"
#endif