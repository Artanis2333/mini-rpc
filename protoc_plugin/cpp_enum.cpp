#include <limits>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>

#include "cpp_enum.h"
#include "plugin_helper.h"

//
// CppEnum
//
CppEnum::CppEnum(const std::string& ns, const std::string& enum_name) :
    namespace_(ns),
    enum_name_(enum_name)
{
}

bool CppEnum::Parse(const google::protobuf::EnumDescriptor* desc, std::string* /*error*/)
{
    proto_name_ = desc->name();
    proto_full_name_ = desc->full_name();

    for (int i = 0; i < desc->value_count(); ++i)
    {
        const google::protobuf::EnumValueDescriptor* value_desc = desc->value(i);
        name_value_pair_.emplace_back(value_desc->name(), value_desc->number());
        if (value2name_.count(value_desc->number()) == 0)
        {
            value2name_[value_desc->number()] = value_desc->name();
        }
    }

    return true;
}

void CppEnum::OutputToHeaderFile(google::protobuf::io::Printer& printer,
        std::map<std::string, std::string>& vars) const
{
    // enum
    vars["enum_name"] = enum_name_;
    printer.Print(vars, "enum $enum_name$ : int32_t\n"
            "{\n");

    int value_min = std::numeric_limits<int>::max();
    int value_max = std::numeric_limits<int>::min();

    // name/value pair
    for (auto& item : name_value_pair_)
    {
        auto& name = item.first;
        auto& value = item.second;

        if (value_min > value)
        {
            value_min = value;
            vars["enum_value_min_name"] = name;
        }
        if (value_max < value)
        {
            value_max = value;
            vars["enum_value_max_name"] = name;
        }

        vars["enum_value_name"] = name;
        vars["enum_value_number"] = std::to_string(value);
        printer.Print(vars, "    $enum_value_name$ = $enum_value_number$,\n");
    }

    // enum
    printer.Print("};\n"
            "\n");

    // const
    printer.Print(vars,
            "constexpr $enum_name$ $enum_name$_MIN = $enum_value_min_name$;\n"
            "constexpr $enum_name$ $enum_name$_MAX = $enum_value_max_name$;\n"
            "constexpr int32_t $enum_name$_ARRAYSIZE = $enum_name$_MAX + 1;\n"
            "\n");

    // methods
    printer.Print(vars,
            "bool $enum_name$_IsValid(int32_t value);\n"
            "std::string_view $enum_name$_Name(int32_t value);\n"
            "bool $enum_name$_Parse(std::string_view name, int32_t& value);\n"
            "const mrpc::EnumDescriptor* $enum_name$_GetDescriptor();\n"
            "\n");
}

void CppEnum::OutputToSourceFile(google::protobuf::io::Printer& printer,
        std::map<std::string, std::string>& vars) const
{
    vars["namespace"] = namespace_;
    vars["enum_name"] = enum_name_;
    vars["proto_name"] = proto_name_;
    vars["proto_name_length"] = std::to_string(proto_name_.length());
    vars["proto_full_name"] = proto_full_name_;
    vars["proto_full_name_length"] = std::to_string(proto_full_name_.length());

    // DescriptorWrapper
    printer.Print(vars,
            "namespace $namespace$::$enum_name$_DescriptorWrapper\n"
            "{\n"
            "static mrpc::EnumDescriptor descriptor = { std::string_view(\"$proto_name$\", $proto_name_length$),\n"
            "    std::string_view(\"$proto_full_name$\", $proto_full_name_length$),\n"
            "    $enum_name$_IsValid,\n"
            "    $enum_name$_Name,\n"
            "    $enum_name$_Parse,\n"
            "};\n"
            "};\n"
            "\n");

    // method IsValid
    printer.Print(vars,
            "bool $namespace$::$enum_name$_IsValid(int32_t value)\n"
            "{\n"
            "    switch (value)\n"
            "    {\n"
            );
    for (auto& [_, name] : value2name_)
    {
        vars["enum_value_name"] = name;
        printer.Print(vars, "        case $enum_value_name$:\n");
    }
    printer.Print(
            "            return true;\n"
            "            break;\n"
            "    }\n"
            "    return false;\n"
            "}\n"
            "\n");

    // method Name
    printer.Print(vars,
            "std::string_view $namespace$::$enum_name$_Name(int32_t value)\n"
            "{\n"
            "    switch (value)\n"
            "    {\n"
            );
    for (auto& [_, name] : value2name_)
    {
        vars["enum_value_name"] = name;
        vars["enum_value_name_length"] = std::to_string(name.length());
        printer.Print(vars, "        case $enum_value_name$: return std::string_view(\"$enum_value_name$\", $enum_value_name_length$);\n");
    }
    printer.Print(
            "    }\n"
            "    return std::string_view();\n"
            "}\n"
            "\n");

    // method Parse
    printer.Print(vars,
            "bool $namespace$::$enum_name$_Parse(std::string_view name, int32_t& value)\n"
            "{\n");
    for (auto& item : name_value_pair_)
    {
        auto& name = item.first;
        vars["enum_value_name"] = name;
        vars["enum_value_name_length"] = std::to_string(name.length());
        printer.Print(vars,
                "    if (name == std::string_view(\"$enum_value_name$\", $enum_value_name_length$))\n"
                "    {\n"
                "        value = $enum_value_name$;\n"
                "        return true;\n"
                "    }\n"
                );
    }
    printer.Print(
            "    return false;\n"
            "}\n"
            "\n");

    // method GetDescriptor
    printer.Print(vars,
            "const mrpc::EnumDescriptor* $namespace$::$enum_name$_GetDescriptor()\n"
            "{\n"
            "    return &$namespace$::$enum_name$_DescriptorWrapper::descriptor;\n"
            "}\n"
            "\n");
}
