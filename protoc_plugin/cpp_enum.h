#pragma once

#include <string>
#include <vector>
#include <map>

namespace google::protobuf
{
class EnumDescriptor;

namespace io
{
class Printer;
};
};

class CppEnum
{
public:
    CppEnum(const std::string& ns, const std::string& enum_name);
    ~CppEnum() = default;

    bool Parse(const google::protobuf::EnumDescriptor* desc, std::string* error);

    void OutputToHeaderFile(google::protobuf::io::Printer& printer,
            std::map<std::string, std::string>& vars) const;
    void OutputToSourceFile(google::protobuf::io::Printer& printer,
            std::map<std::string, std::string>& vars) const;

    inline void SetGenCppReflection(bool value) { gen_cpp_reflection_ = value; }

private:
    std::string namespace_;
    std::string enum_name_;
    std::string proto_name_;
    std::string proto_full_name_;
    std::vector<std::pair<std::string, int>> name_value_pair_;
    std::map<int, std::string> value2name_;
    bool gen_cpp_reflection_ = false;
};
