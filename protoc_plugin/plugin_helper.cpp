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

bool IsClassIgnored(const std::string &class_full_name)
{
    return class_full_name == "mrpc.Void";
}
