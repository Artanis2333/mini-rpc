#pragma once

#include <cstdint>
#include <string>

namespace google::protobuf
{
class MethodDescriptor;

namespace io
{
class Printer;
};
};

class CppMethod
{
public:
    CppMethod(const std::string& ns, const std::string& service_name, const std::string& method_name);
    ~CppMethod() = default;

    bool Parse(const google::protobuf::MethodDescriptor* desc, std::string* error);

    void OutputInterfaceMethodDefinition(google::protobuf::io::Printer& printer,
            std::map<std::string, std::string>& vars) const;

    void OutputStubSyncMethodDefinition(google::protobuf::io::Printer& printer,
            std::map<std::string, std::string>& vars) const;
    void OutputStubAsyncMethodDefinition(google::protobuf::io::Printer& printer,
            std::map<std::string, std::string>& vars) const;

    void OutputInterfaceNameBasedCallMethodImplementation(google::protobuf::io::Printer& printer,
            std::map<std::string, std::string>& vars) const;
    void OutputInterfaceMrpcInternalCallMethodImplementation(google::protobuf::io::Printer& printer,
            std::map<std::string, std::string>& vars) const;

    void OutputStubSyncMethodImplementation(google::protobuf::io::Printer& printer,
            std::map<std::string, std::string>& vars) const;
    void OutputStubAsyncMethodImplementation(google::protobuf::io::Printer& printer,
            std::map<std::string, std::string>& vars) const;

private:
    std::string namespace_;
    std::string service_name_;
    std::string method_name_;
    uint32_t method_name_hash_ = 0;

    std::string input_type_name_;
    std::string input_type_full_name_;
    std::string input_type_proto_full_name_;
    bool input_type_same_namespace_ = true;

    std::string output_type_name_;
    std::string output_type_full_name_;
    std::string output_type_proto_full_name_;
    bool output_type_same_namespace_ = true;

    friend class CppService;
};
