#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>

#include "cpp_method.h"
#include "plugin_helper.h"

CppMethod::CppMethod(const std::string& ns, const std::string& service_name, const std::string& method_name) :
    namespace_(ns),
    service_name_(service_name),
    method_name_(method_name)
{
    method_name_hash_ = APHash(method_name_);
}

bool CppMethod::Parse(const google::protobuf::MethodDescriptor* desc, std::string* /*error*/)
{
    const google::protobuf::Descriptor* input_type_desc = desc->input_type();
    const google::protobuf::Descriptor* output_type_desc = desc->output_type();

    input_type_name_ = input_type_desc->name();
    input_type_full_name_ = DotNameToCppTypeName(input_type_desc->full_name());
    input_type_proto_full_name_ = input_type_desc->full_name();
    if (namespace_ + "::" + input_type_name_ != input_type_full_name_)
    {
        input_type_same_namespace_ = false;
    }

    output_type_name_ = output_type_desc->name();
    output_type_full_name_ = DotNameToCppTypeName(output_type_desc->full_name());
    output_type_proto_full_name_ = output_type_desc->full_name();
    if (namespace_ + "::" + output_type_name_ != output_type_full_name_)
    {
        output_type_same_namespace_ = false;
    }

    return true;
}

void CppMethod::OutputInterfaceMethodDefinition(google::protobuf::io::Printer& printer,
        std::map<std::string, std::string>& vars) const
{
    vars["method_name"] = method_name_;
    vars["input_type_name"] = input_type_same_namespace_ ? input_type_name_ : input_type_full_name_;
    vars["output_type_name"] = output_type_same_namespace_ ? output_type_name_ : output_type_full_name_;
    printer.Print(vars, "    virtual int32_t $method_name$(const $input_type_name$& req, $output_type_name$& rsp) = 0;\n");
}

void CppMethod::OutputStubSyncMethodDefinition(google::protobuf::io::Printer& printer,
        std::map<std::string, std::string>& vars) const
{
    vars["method_name"] = method_name_;
    vars["input_type_name"] = input_type_same_namespace_ ? input_type_name_ : input_type_full_name_;
    vars["output_type_name"] = output_type_same_namespace_ ? output_type_name_ : output_type_full_name_;
    printer.Print(vars, "    int32_t $method_name$(const $input_type_name$& req, $output_type_name$& rsp);\n");
}

void CppMethod::OutputStubAsyncMethodDefinition(google::protobuf::io::Printer& printer,
        std::map<std::string, std::string>& vars) const
{
    vars["method_name"] = method_name_;
    vars["input_type_name"] = input_type_same_namespace_ ? input_type_name_ : input_type_full_name_;
    vars["output_type_name"] = output_type_same_namespace_ ? output_type_name_ : output_type_full_name_;
    printer.Print(vars, "    void $method_name$_Async(const $input_type_name$& req, const std::shared_ptr<mrpc::AsyncCallback<$output_type_name$>>& cb);\n");
}

void CppMethod::OutputInterfaceNameBasedCallMethodImplementation(google::protobuf::io::Printer& printer,
        std::map<std::string, std::string>& vars) const
{
    vars["method_name"] = method_name_;
    vars["input_type_name"] = input_type_full_name_;
    vars["output_type_name"] = output_type_full_name_;
    printer.Print(vars,
            "        case $method_index$:\n"
            "            return $method_name$(dynamic_cast<const $input_type_name$&>(req), dynamic_cast<$output_type_name$&>(rsp));\n"
            "            break;\n"
            );
}

void CppMethod::OutputInterfaceMrpcInternalCallMethodImplementation(google::protobuf::io::Printer& printer,
        std::map<std::string, std::string>& vars) const
{
    vars["method_name"] = method_name_;
    vars["input_type_name"] = input_type_full_name_;
    vars["output_type_name"] = output_type_full_name_;
    printer.Print(vars,
            "        case $method_index$:\n"
            "        {\n"
            "            $input_type_name$ req;\n"
            "            if (!req.ParseFromString(req_data))\n"
            "            {\n"
            "                return mrpc::ERROR_INVALID_METHOD_REQUEST_DATA;\n"
            "            }\n"
            "\n"
            "            $output_type_name$ rsp;\n"
            "            ret = $method_name$(req, rsp);\n"
            "            if (ret != 0)\n"
            "            {\n"
            "                return ret;\n"
            "            }\n"
            "\n"
            "            rsp.SerializeToString(rsp_data);\n"
            "            return 0;\n"
            "            break;\n"
            "        }\n"
            );
}

void CppMethod::OutputStubSyncMethodImplementation(google::protobuf::io::Printer& printer,
        std::map<std::string, std::string>& vars) const
{
    vars["namespace"] = namespace_;
    vars["service_name"] = service_name_;
    vars["method_name"] = method_name_;
    vars["method_name_hash"] = std::to_string(method_name_hash_);
    vars["input_type_name"] = input_type_full_name_;
    vars["output_type_name"] = output_type_full_name_;
    printer.Print(vars, "int32_t $namespace$::$service_name$Stub::$method_name$(const $input_type_name$& req, $output_type_name$& rsp)\n"
            "{\n"
            "    std::string req_data, rsp_data;\n"
            "    req.SerializeToString(req_data);\n"
            "    int32_t ret = CallMethod($method_name_hash$u, req_data, rsp_data);\n"
            "    if (ret != 0)\n"
            "    {\n"
            "        return ret;\n"
            "    }\n"
            "    if (!rsp.ParseFromString(rsp_data))\n"
            "    {\n"
            "        return mrpc::ERROR_INVALID_METHOD_RESPONSE_DATA;\n"
            "    }\n"
            "    return 0;\n"
            "}\n"
            "\n");
}

void CppMethod::OutputStubAsyncMethodImplementation(google::protobuf::io::Printer& printer,
        std::map<std::string, std::string>& vars) const
{
    vars["namespace"] = namespace_;
    vars["service_name"] = service_name_;
    vars["method_name"] = method_name_;
    vars["input_type_name"] = input_type_full_name_;
    vars["output_type_name"] = output_type_full_name_;
    printer.Print(vars, "void $namespace$::$service_name$Stub::$method_name$_Async(const $input_type_name$& req, const std::shared_ptr<mrpc::AsyncCallback<$output_type_name$>>& cb)\n"
            "{\n"
            "    std::string req_data;\n"
            "    req.SerializeToString(req_data);\n"
            "    CallMethod($method_name_hash$u, req_data, cb);\n"
            "}\n"
            "\n");
}
