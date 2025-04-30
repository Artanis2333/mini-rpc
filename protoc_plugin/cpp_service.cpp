#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>

#include "cpp_service.h"
#include "plugin_helper.h"

CppService::CppService(const std::string& ns, const std::string& service_name) :
    namespace_(ns),
    service_name_(service_name)
{
}

bool CppService::Parse(const google::protobuf::ServiceDescriptor* desc, std::string* error)
{
    service_proto_full_name_ = desc->full_name();
    service_proto_full_name_hash_ = APHash(service_proto_full_name_);

    // methods
    for (int i = 0; i < desc->method_count(); ++i)
    {
        const google::protobuf::MethodDescriptor* method_desc = desc->method(i);
        CppMethod& method = methods_.emplace_back(namespace_, service_name_, method_desc->name());
        if (!method.Parse(method_desc, error))
        {
            return false;
        }
    }

    return true;
}

void CppService::OutputToHeaderFile(google::protobuf::io::Printer& printer,
        std::map<std::string, std::string>& vars) const
{
    vars["service_name"] = service_name_;

    // service interface
    printer.Print(vars, "class $service_name$ : public mrpc::Service\n");
    printer.Print("{\n"
            "public:\n");

    // methods
    for (auto& method : methods_)
    {
        method.OutputInterfaceMethodDefinition(printer, vars);
    }
    printer.Print("\n");

    printer.Print(
            "    // implements Service ----------------------------------------------\n"
            "\n"
            "    const mrpc::Descriptor* GetRequestDescriptor(const std::string& method_name) const override;\n"
            "    const mrpc::Descriptor* GetResponseDescriptor(const std::string& method_name) const override;\n"
            "    int32_t CallMethod(const std::string& method_name, const mrpc::Message& req, mrpc::Message& rsp) override;\n"
            "\n"
            "    int32_t CallMethod(uint32_t method_code, const std::string& req_data, std::string& rsp_data) override;\n"
            "\n");

    // service interface
    printer.Print("private:\n"
            "    const static std::map<std::string_view, int32_t> kMethodNameToIndex;\n"
            "    const static std::map<uint32_t, int32_t> kMethodNameHashToIndex;\n"
            "    const static std::map<std::string_view, std::string_view> kMethodNameToRequestDescriptorName;\n"
            "    const static std::map<std::string_view, std::string_view> kMethodNameToResponseDescriptorName;\n"
            "};\n"
            "\n");

    // service stub 
    printer.Print(vars, "class $service_name$Stub final : public mrpc::ServiceStub\n");
    printer.Print("{\n"
            "public:\n");

    // methods
    for (auto& method : methods_)
    {
        method.OutputStubSyncMethodDefinition(printer, vars);
    }
    printer.Print("\n");
    for (auto& method : methods_)
    {
        method.OutputStubAsyncMethodDefinition(printer, vars);
    }
    printer.Print("\n");

    // service stub 
    printer.Print("};\n"
            "\n");
}

void CppService::OutputToSourceFile(google::protobuf::io::Printer& printer,
        std::map<std::string, std::string>& vars) const
{
    vars["namespace"] = namespace_;
    vars["service_name"] = service_name_;

    // service static field kMethodNameToIndex
    printer.Print(vars, "const std::map<std::string_view, int32_t> $namespace$::$service_name$::kMethodNameToIndex = {\n");
    for (size_t i = 0; i < methods_.size(); ++i)
    {
        vars["method_name"] = methods_[i].method_name_;
        vars["method_name_length"] = std::to_string(methods_[i].method_name_.length());
        vars["method_index"] = std::to_string(i);
        printer.Print(vars, "    { std::string_view(\"$method_name$\", $method_name_length$), $method_index$ }");
        if (i != methods_.size() - 1)
        {
            printer.Print(",\n");
        }
        else
        {
            printer.Print("\n");
        }
    }
    printer.Print("};\n"
            "\n");

    // service static field kMethodNameHashToIndex
    printer.Print(vars, "const std::map<uint32_t, int32_t> $namespace$::$service_name$::kMethodNameHashToIndex = {\n");
    for (size_t i = 0; i < methods_.size(); ++i)
    {
        vars["method_name_hash"] = std::to_string(methods_[i].method_name_hash_);
        vars["method_index"] = std::to_string(i);
        printer.Print(vars, "    { $method_name_hash$u, $method_index$ }");
        if (i != methods_.size() - 1)
        {
            printer.Print(",\n");
        }
        else
        {
            printer.Print("\n");
        }
    }
    printer.Print("};\n"
            "\n");

    // service static field kMethodNameToRequestDescriptorName
    printer.Print(vars, "const std::map<std::string_view, std::string_view> $namespace$::$service_name$::kMethodNameToRequestDescriptorName = {\n");
    for (size_t i = 0; i < methods_.size(); ++i)
    {
        vars["method_name"] = methods_[i].method_name_;
        vars["method_name_length"] = std::to_string(methods_[i].method_name_.length());
        vars["input_type_proto_name"] = methods_[i].input_type_proto_full_name_;
        vars["input_type_proto_name_length"] = std::to_string(methods_[i].input_type_proto_full_name_.length());
        printer.Print(vars, "    { std::string_view(\"$method_name$\", $method_name_length$), std::string_view(\"$input_type_proto_name$\", $input_type_proto_name_length$) }");
        if (i != methods_.size() - 1)
        {
            printer.Print(",\n");
        }
        else
        {
            printer.Print("\n");
        }
    }
    printer.Print("};\n"
            "\n");

    // service static field kMethodNameToResponseDescriptorName
    printer.Print(vars, "const std::map<std::string_view, std::string_view> $namespace$::$service_name$::kMethodNameToResponseDescriptorName = {\n");
    for (size_t i = 0; i < methods_.size(); ++i)
    {
        vars["method_name"] = methods_[i].method_name_;
        vars["method_name_length"] = std::to_string(methods_[i].method_name_.length());
        vars["output_type_proto_name"] = methods_[i].output_type_proto_full_name_;
        vars["output_type_proto_name_length"] = std::to_string(methods_[i].output_type_proto_full_name_.length());
        printer.Print(vars, "    { std::string_view(\"$method_name$\", $method_name_length$), std::string_view(\"$output_type_proto_name$\", $output_type_proto_name_length$) }");
        if (i != methods_.size() - 1)
        {
            printer.Print(",\n");
        }
        else
        {
            printer.Print("\n");
        }
    }
    printer.Print("};\n"
            "\n");

    // service method GetRequestDescriptor
    printer.Print(vars, 
            "const mrpc::Descriptor* $namespace$::$service_name$::GetRequestDescriptor(const std::string& method_name) const\n"
            "{\n"
            "    auto it = kMethodNameToRequestDescriptorName.find(method_name);\n"
            "    if (it == kMethodNameToRequestDescriptorName.end())\n"
            "    {\n"
            "        return nullptr;\n"
            "    }\n"
            "    return mrpc::DescriptorPool::FindDescriptorByFullName(it->second);\n"
            "}\n"
            "\n");

    // service method GetResponseDescriptor
    printer.Print(vars, 
            "const mrpc::Descriptor* $namespace$::$service_name$::GetResponseDescriptor(const std::string& method_name) const\n"
            "{\n"
            "    auto it = kMethodNameToResponseDescriptorName.find(method_name);\n"
            "    if (it == kMethodNameToResponseDescriptorName.end())\n"
            "    {\n"
            "        return nullptr;\n"
            "    }\n"
            "    return mrpc::DescriptorPool::FindDescriptorByFullName(it->second);\n"
            "}\n"
            "\n");

    // service method CallMethod
    printer.Print(vars, 
            "int32_t $namespace$::$service_name$::CallMethod(const std::string& method_name, const mrpc::Message& req, mrpc::Message& rsp)\n"
            "{\n"
            "    auto it = kMethodNameToIndex.find(method_name);\n"
            "    if (it == kMethodNameToIndex.end())\n"
            "    {\n"
            "        return mrpc::ERROR_INVALID_METHOD_NAME;\n"
            "    }\n"
            "\n"
            "    switch (it->second)\n"
            "    {\n"
            );
    for (size_t i = 0; i < methods_.size(); ++i)
    {
        vars["method_index"] = std::to_string(i);
        methods_[i].OutputInterfaceNameBasedCallMethodImplementation(printer, vars);
    }
    printer.Print("    }\n"
            "    return mrpc::ERROR_INVALID_METHOD_NAME;\n"
            "}\n"
            "\n");

    // service method CallMethod
    printer.Print(vars, 
            "int32_t $namespace$::$service_name$::CallMethod(uint32_t method_code, const std::string& req_data, std::string& rsp_data)\n"
            "{\n"
            "    auto it = kMethodNameHashToIndex.find(method_code);\n"
            "    if (it == kMethodNameHashToIndex.end())\n"
            "    {\n"
            "        return mrpc::ERROR_INVALID_METHOD_NAME_HASH;\n"
            "    }\n"
            "\n"
            "    int32_t ret = 0;\n"
            "    switch (it->second)\n"
            "    {\n"
            );
    for (size_t i = 0; i < methods_.size(); ++i)
    {
        vars["method_index"] = std::to_string(i);
        methods_[i].OutputInterfaceMrpcInternalCallMethodImplementation(printer, vars);
    }
    printer.Print("    }\n"
            "    return mrpc::ERROR_INVALID_METHOD_NAME_HASH;\n"
            "}\n"
            "\n");

    // stub methods
    for (auto& method : methods_)
    {
        vars["service_name_hash"] = std::to_string(service_proto_full_name_hash_);
        method.OutputStubSyncMethodImplementation(printer, vars);
    }
    for (auto& method : methods_)
    {
        method.OutputStubAsyncMethodImplementation(printer, vars);
    }
}

