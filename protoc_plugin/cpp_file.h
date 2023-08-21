#pragma once

#include <string>
#include <string_view>
#include <vector>

#include <mrpc/options.pb.h>

#include "cpp_class.h"
#include "cpp_enum.h"

namespace google::protobuf
{
class FileDescriptor;

namespace compiler
{
class GeneratorContext;
};
};

class CppFile
{
public:
    CppFile(std::string_view file_name);
    ~CppFile() = default;

    bool Parse(const google::protobuf::FileDescriptor* file_desc, std::string* error);

    void OutputToHeaderFile(google::protobuf::compiler::GeneratorContext* ctx) const;
    void OutputToSourceFile(google::protobuf::compiler::GeneratorContext* ctx) const;

private:
    std::string proto_file_name_;
    std::string header_file_name_;
    std::string source_file_name_;
    std::vector<std::string> includes_;
    std::string namespace_;
    std::vector<CppEnum> enums_;
    std::vector<CppClass> classes_;
    bool gen_cpp_reflection_ = false;

    std::string MakeIncludeMarco() const;
    bool NeedIncludeCppTypeHeader(mrpc::CppType cpp_type) const;
};
