#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <map>

#include <google/protobuf/descriptor.h>

#include <mrpc/options.pb.h>

namespace google::protobuf
{
class FieldDescriptor;

namespace io
{
class Printer;
};
};

class CppField
{
public:
    CppField(const std::string& field_name);
    ~CppField() = default;

    bool Parse(const google::protobuf::FieldDescriptor* desc, std::string* error);

    void OutputFieldDefinition(google::protobuf::io::Printer& printer,
            std::map<std::string, std::string>& vars) const;
    void OutputFieldCacheSize(google::protobuf::io::Printer& printer,
            std::map<std::string, std::string>& vars) const;

    void OutputClearMethod(google::protobuf::io::Printer& printer,
            std::map<std::string, std::string>& vars) const;

    void OutputByteSizeSkipDefaultMethod(google::protobuf::io::Printer& printer,
            std::map<std::string, std::string>& vars) const;
    void OutputByteSizeNotSkipDefaultMethod(google::protobuf::io::Printer& printer,
            std::map<std::string, std::string>& vars) const;

    void OutputSerializeToStringSkipDefaultMethod(google::protobuf::io::Printer& printer,
            std::map<std::string, std::string>& vars) const;
    void OutputSerializeToStringNotSkipDefaultMethod(google::protobuf::io::Printer& printer,
            std::map<std::string, std::string>& vars) const;

    void OutputParseFromBytesMethod(google::protobuf::io::Printer& printer,
            std::map<std::string, std::string>& vars) const;

    void OutputDescriptorWrapperMember(google::protobuf::io::Printer& printer,
            std::map<std::string, std::string>& vars) const;

    void OutputDescriptorInitializerList(google::protobuf::io::Printer& printer,
            std::map<std::string, std::string>& vars) const;

private:
    std::string field_name_;
    int tag_number_ = 0;
    int proto_type_ = 0;
    int proto_sub_type_1_ = 0;
    int proto_sub_type_2_ = 0;
    mrpc::CppType cpp_type_ = mrpc::CPPTYPE_UNKNOWN;
    mrpc::CppType cpp_sub_type_1_ = mrpc::CPPTYPE_UNKNOWN;
    mrpc::CppType cpp_sub_type_2_ = mrpc::CPPTYPE_UNKNOWN;
    std::string field_type_name_;
    std::string field_default_;


    static mrpc::CppType const kPbCppTypeToMrpcCppType[google::protobuf::FieldDescriptor::MAX_CPPTYPE + 1];
    static const std::string_view kCppTypeToEnumName[mrpc::CppType_ARRAYSIZE];
    static const std::string_view kCppTypeToName[mrpc::CppType_ARRAYSIZE];
    static const std::string_view kCppTypeDefault[mrpc::CppType_ARRAYSIZE];
    static const std::map<int, std::string_view> kPbTypeToTemplateType;

    static bool IsNamedType(mrpc::CppType cpp_type);
    static bool IsContainerType(mrpc::CppType cpp_type);
    static bool IsSequenceContainerType(mrpc::CppType cpp_type);
    static bool IsAssociativeContainerType(mrpc::CppType cpp_type);

    friend class CppClass;
};
