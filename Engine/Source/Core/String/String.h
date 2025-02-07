#pragma once

#include "Core/Core.h"
#include "Core/Types.h"

const uchar* operator""_uchar(const char* str, SIZE_T strSize);

#define STRING(text) text##_uchar
#define type_to_string(type) std::string(#type)
#define var_to_string(type, value) std::string(" [") + std::string(#type) + std::string("] ") + std::string(#value) + std::string(" = ") + std::to_string(value) + std::string(";")
#define var_to_string_n(type, value, name) std::string(" [") + std::string(#type) + std::string("] ") + std::string(name) + std::string(" = ") + std::to_string(value) + std::string(";")