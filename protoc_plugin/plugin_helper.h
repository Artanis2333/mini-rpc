#pragma once

#include <cstdint>
#include <string>
#include <string_view>

bool IsImportIgnored(const std::string &dot_name);

std::string DotNameToCppTypeName(const std::string &dot_name);

uint32_t APHash(std::string_view str);
