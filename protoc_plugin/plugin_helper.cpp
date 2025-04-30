#include "plugin_helper.h"

bool IsImportIgnored(const std::string &dot_name)
{
    if (dot_name.find("google/protobuf/") != std::string::npos ||
            dot_name.find("mrpc/options.proto") != std::string::npos)
    {
        return true;
    }
    return false;
}

std::string DotNameToCppTypeName(const std::string &dot_name)
{
    std::string type_name;
    for (auto c : dot_name)
    {
        if (c == '.')
        {
            type_name.append("::");
        }
        else
        {
            type_name.push_back(c);
        }
    }
    return type_name;
}

uint32_t APHash(std::string_view str)
{
    uint32_t hash = 0;
    for (size_t i = 0; i < str.length(); i++)
    {
        if ((i & 1) == 0)
        {
            hash ^= ((hash << 7) ^ str[i] ^ (hash >> 3));
        }
        else
        {
            hash ^= (~((hash << 11) ^ str[i] ^ (hash >> 5)));
        }
    }
    return hash;
}
