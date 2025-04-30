#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace mrpc
{

class String
{
public:
    static std::vector<std::string_view> Split(std::string_view str, char delim, bool skip_empty = true);
    static std::vector<std::string_view> Split(std::string_view str, std::string_view delim, bool skip_empty = true);

    static std::vector<std::string> Split(const std::string& str, char delim, bool skip_empty = true);
    static std::vector<std::string> Split(const std::string& str, const std::string& delim, bool skip_empty = true);
};

}
