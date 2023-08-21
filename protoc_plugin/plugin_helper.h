#pragma once

#include <string>

bool IsImportIgnored(const std::string &dot_name);

std::string DotNameToCppTypeName(const std::string &dot_name);

bool IsClassIgnored(const std::string &class_full_name);

