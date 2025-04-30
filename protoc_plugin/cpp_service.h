#pragma once

#include <string>
#include <vector>

#include "cpp_method.h"

namespace google::protobuf
{
class ServiceDescriptor;

namespace io
{
class Printer;
};
};

class CppService
{
public:
    CppService(const std::string& ns, const std::string& service_name);
    ~CppService() = default;

    bool Parse(const google::protobuf::ServiceDescriptor* desc, std::string* error);

    void OutputToHeaderFile(google::protobuf::io::Printer& printer,
            std::map<std::string, std::string>& vars) const;
    void OutputToSourceFile(google::protobuf::io::Printer& printer,
            std::map<std::string, std::string>& vars) const;

private:
    std::string namespace_;
    std::string service_name_;
    std::string service_proto_full_name_;
    uint32_t service_proto_full_name_hash_;
    std::vector<CppMethod> methods_;
};
