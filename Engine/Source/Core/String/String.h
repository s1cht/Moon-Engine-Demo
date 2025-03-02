#pragma once

#include "Core/Core.h"
#include "Core/CoreTypes.h"
#include "Macros.h"

template <class arr>
uchar* ConvertArrayToStr(arr str)
{
	uchar* newStr = &*str.Begin();

	return newStr;
}


#define type_to_string(type) std::string(#type)
#define var_to_string(type, value) std::string(" [") + std::string(#type) + std::string("] ") + std::string(#value) + std::string(" = ") + std::to_string(value) + std::string(";")
#define var_to_string_n(type, value, name) std::string(" [") + std::string(#type) + std::string("] ") + std::string(name) + std::string(" = ") + std::to_string(value) + std::string(";")