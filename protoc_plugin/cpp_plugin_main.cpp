#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/compiler/plugin.h>

#include <mrpc/options.pb.h>

#include "cpp_file.h"

class MrpcCppGenerator : public google::protobuf::compiler::CodeGenerator
{
public:
    bool Generate(const google::protobuf::FileDescriptor* file_desc,
            const std::string& parameter,
            google::protobuf::compiler::GeneratorContext* ctx,
            std::string* error) const override;

    uint64_t GetSupportedFeatures() const override;
};

bool MrpcCppGenerator::Generate(const google::protobuf::FileDescriptor* file_desc,
        const std::string& /*parameter*/,
        google::protobuf::compiler::GeneratorContext* ctx,
        std::string* error) const
{
    const std::string& proto_file_name = file_desc->name();
    const size_t pos = proto_file_name.find_last_of('.');
    if (pos == std::string::npos)
    {
        *error = "Invalid file name: " + proto_file_name;
        return false;
    }

    std::string_view cpp_file_name(proto_file_name.c_str(), pos);
    CppFile cpp_file(cpp_file_name);
    if (!cpp_file.Parse(file_desc, error))
    {
        return false;
    }

    cpp_file.OutputToHeaderFile(ctx);
    cpp_file.OutputToSourceFile(ctx);

    return true;
}

uint64_t MrpcCppGenerator::GetSupportedFeatures() const
{
    return FEATURE_PROTO3_OPTIONAL;
}

int main(int argc, char* argv[])
{
    MrpcCppGenerator generator;
    return google::protobuf::compiler::PluginMain(argc, argv, &generator);
}
