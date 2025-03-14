#pragma once

#include "Core/Utils/Logging/Logger.h"

#ifdef _DEBUG
	#define PE_ASSERT(x, ...)			\
	if (!(x))							\
	{									\
		PE_CRITICAL(__VA_ARGS__);		\
		__debugbreak();					\
	}										


	#define PE_CORE_ASSERT(x, ...)		\
	if (!(x))							\
	{									\
		PE_CORE_CRITICAL(__VA_ARGS__);	\
		__debugbreak();					\
	}

#else
	#define PE_ASSERT(x, ...)
	#define PE_CORE_ASSERT(x, ...)
#endif