#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>

#include "cpp_class.h"

//
// CppClass
//
CppClass::CppClass(const std::string& ns, const std::string& class_name) :
    namespace_(ns),
    class_name_(class_name)
{
}

bool CppClass::Parse(const google::protobuf::Descriptor* desc, std::string* error)
{
    proto_name_ = desc->name();
    proto_full_name_ = desc->full_name();

    // fields
    for (int i = 0; i < desc->field_count(); ++i)
    {
        const google::protobuf::FieldDescriptor* field_desc = desc->field(i);
        CppField& field = fields_.emplace_back(field_desc->name());
        if (!field.Parse(field_desc, error))
        {
            return false;
        }
    }

    return true;
}

void CppClass::OutputToHeaderFile(google::protobuf::io::Printer& printer,
        std::map<std::string, std::string>& vars) const
{
    // class
    vars["class_name"] = class_name_;

    if (gen_cpp_reflection_)
    {
        printer.Print(vars, "class $class_name$ final : public mrpc::ReflectableMessage<$class_name$>\n");
    }
    else
    {
        printer.Print(vars, "class $class_name$ final : public mrpc::Message\n");
    }
    printer.Print("{\n"
            "public:\n");

    // fields
    for (auto& field : fields_)
    {
        field.OutputFieldDefinition(printer, vars);
    }
    printer.Print("\n");

    // methods
    printer.Print(
            "    std::string_view GetName() const override;\n"
            "    std::string_view GetFullName() const override;\n");
    if (gen_cpp_reflection_)
    {
        printer.Print("    const mrpc::Descriptor* GetDescriptor() const override;\n");
        printer.Print("    static const mrpc::Descriptor* GetClassDescriptor();\n");
    }
    printer.Print("    void Clear() override;\n"
            "\n");

    // class
    printer.Print("protected:\n");

    // methods
    printer.Print(
            "    size_t ByteSizeSkipDefault() override;\n"
            "    size_t ByteSizeNotSkipDefault() override;\n"
            "    void SerializeToStringSkipDefault(std::string& s) override;\n"
            "    void SerializeToStringNotSkipDefault(std::string& s) override;\n"
            "    bool ParseFromBytes(const uint8_t* begin, const uint8_t* const end) override;\n"
            "\n");

    // class
    printer.Print("private:\n");

    // fields
    for (auto& field : fields_)
    {
        field.OutputFieldCacheSize(printer, vars);
    }

    // class
    printer.Print("};\n"
            "\n");
}

void CppClass::OutputToSourceFile(google::protobuf::io::Printer& printer,
        std::map<std::string, std::string>& vars) const
{
    // class
    vars["namespace"] = namespace_;
    vars["class_name"] = class_name_;
    vars["proto_name"] = proto_name_;
    vars["proto_name_length"] = std::to_string(proto_name_.length());
    vars["proto_full_name"] = proto_full_name_;
    vars["proto_full_name_length"] = std::to_string(proto_full_name_.length());

    // DescriptorWrapper
    if (gen_cpp_reflection_)
    {
        printer.Print(vars,
                "namespace $namespace$::$class_name$_DescriptorWrapper\n"
                "{\n"
                );
        for (auto& field : fields_)
        {
            field.OutputDescriptorWrapperMember(printer, vars);
        }
        printer.Print(vars,
                "static mrpc::DescriptorImpl<$class_name$> descriptor = { std::string_view(\"$proto_name$\", $proto_name_length$),\n"
                "    std::string_view(\"$proto_full_name$\", $proto_full_name_length$),\n"
                "    {\n"
                );
        for (auto& field : fields_)
        {
            field.OutputDescriptorInitializerList(printer, vars);
        }
        printer.Print("    }\n"
                "};\n"
                "};\n"
                "\n");
    }

    // method GetName
    printer.Print(vars, "std::string_view $namespace$::$class_name$::GetName() const\n"
            "{\n"
            "    return std::string_view(\"$proto_name$\", $proto_name_length$);\n"
            "}\n"
            "\n");

    // method GetFullName
    printer.Print(vars, "std::string_view $namespace$::$class_name$::GetFullName() const\n"
            "{\n"
            "    return std::string_view(\"$proto_full_name$\", $proto_full_name_length$);\n"
            "}\n"
            "\n");

    // method GetDescriptor
    if (gen_cpp_reflection_)
    {
        printer.Print(vars, "const mrpc::Descriptor* $namespace$::$class_name$::GetDescriptor() const\n"
                "{\n"
                "    return GetClassDescriptor();\n"
                "}\n"
                "\n");
        printer.Print(vars, "const mrpc::Descriptor* $namespace$::$class_name$::GetClassDescriptor()\n"
                "{\n"
                "    return &$class_name$_DescriptorWrapper::descriptor;\n"
                "}\n"
                "\n");
    }

    // method Clear
    printer.Print(vars, "void $namespace$::$class_name$::Clear()\n"
            "{\n");
    for (auto& field : fields_)
    {
        field.OutputClearMethod(printer, vars);
    }
    printer.Print("}\n"
            "\n");

    // method ByteSize
    printer.Print(vars, "size_t $namespace$::$class_name$::ByteSizeSkipDefault()\n"
            "{\n"
            "    size_t size = 0;\n");
    for (auto& field : fields_)
    {
        field.OutputByteSizeSkipDefaultMethod(printer, vars);
    }
    printer.Print("    cached_size_ = size;\n"
            "    return size;\n"
            "}\n"
            "\n");

    // method ByteSize
    printer.Print(vars, "size_t $namespace$::$class_name$::ByteSizeNotSkipDefault()\n"
            "{\n"
            "    size_t size = 0;\n");
    for (auto& field : fields_)
    {
        field.OutputByteSizeNotSkipDefaultMethod(printer, vars);
    }
    printer.Print("    cached_size_ = size;\n"
            "    return size;\n"
            "}\n"
            "\n");

    // method SerializeToString
    printer.Print(vars, "void $namespace$::$class_name$::SerializeToStringSkipDefault(std::string& s)\n"
            "{\n");
    for (auto& field : fields_)
    {
        field.OutputSerializeToStringSkipDefaultMethod(printer, vars);
    }
    printer.Print("}\n"
            "\n");

    // method SerializeToString
    printer.Print(vars, "void $namespace$::$class_name$::SerializeToStringNotSkipDefault(std::string& s)\n"
            "{\n");
    for (auto& field : fields_)
    {
        field.OutputSerializeToStringNotSkipDefaultMethod(printer, vars);
    }
    printer.Print("}\n"
            "\n");

    // method ParseFromBytes
    printer.Print(vars, "bool $namespace$::$class_name$::ParseFromBytes(const uint8_t* begin, const uint8_t* const end)\n"
            "{\n"
            "    bool result = true;\n"
            "    uint32_t tag = 0, type = 0;\n"
            "    while ((begin < end) && (result = mrpc::ParseTag(tag, type, begin, end)))\n"
            "    {\n"
            "        switch (tag)\n"
            "        {\n"
            "");
    for (auto& field : fields_)
    {
        field.OutputParseFromBytesMethod(printer, vars);
    }
    printer.Print(
            "            default: if (!mrpc::ParseSkipUnknown(type, begin, end)) return false;\n"
            "                break;\n"
            "        }\n"
            "    }\n"
            "    return result;\n"
            "}\n"
            "\n");
}

bool CppClass::HasCppTypeField(mrpc::CppType cpp_type) const
{
    for (auto& field : fields_)
    {
        if (field.cpp_type_ == cpp_type) return true;
    }

    return false;
}
