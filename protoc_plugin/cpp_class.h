#pragma once

#include <string>
#include <vector>
#include <map>

#include "cpp_field.h"

namespace google::protobuf
{
class Descriptor;

namespace io
{
class Printer;
};
};

class CppClass
{
public:
    CppClass(const std::string& ns, const std::string& class_name);
    ~CppClass() = default;

    bool Parse(const google::protobuf::Descriptor* desc, std::string* error);

    void OutputToHeaderFile(google::protobuf::io::Printer& printer,
            std::map<std::string, std::string>& vars) const;
    void OutputToSourceFile(google::protobuf::io::Printer& printer,
            std::map<std::string, std::string>& vars) const;

    bool HasCppTypeField(mrpc::CppType cpp_type) const;

private:
    std::string namespace_;
    std::string class_name_;
    std::string proto_name_;
    std::string proto_full_name_;
    std::vector<CppField> fields_;
};
