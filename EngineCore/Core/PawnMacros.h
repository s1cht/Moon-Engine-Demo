#pragma once

// Bitwise operations

#define BIT(x) (1 << x)

#define PE_DEPRECATED [[deprecated]]
#define PE_DEPRECATED_M(text) [[deprecated(text)]]

#define BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)


#include "Core/Events/EventMacros.h"
#include "Core/Containers/String/StringMacros.h"