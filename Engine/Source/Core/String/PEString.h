#pragma once

#include <string>

#define var_to_string(type, value) std::string(" [") + std::string(#type) + std::string("] ") + std::string(#value) + std::string(" = ") + std::to_string(value) + std::string(";")
#define var_to_string_n(type, value, name) std::string(" [") + std::string(#type) + std::string("] ") + std::string(name) + std::string(" = ") + std::to_string(value) + std::string(";")