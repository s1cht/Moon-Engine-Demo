#pragma once

#include <Core.hpp>

#define PE_D3D11_CHECK(x)		\
if (FAILED(x))					\
{								\
	Shutdown();					\
	PE_ASSERT(false,			\
TEXT(							\
"Assertion failed! Error: {0}"	\
),								\
(int32)x);						\
	return x;					\
}

#define PE_D3D11_RELEASE(x)		\
if (x != nullptr)				\
{								\
	x->Release();				\
	x = nullptr;				\
}

#define PE_D3D11_SHUTDOWN(x)	\
if (x != nullptr)				\
{								\
	x->Shutdown();				\
	delete x;					\
	x = nullptr;				\
}