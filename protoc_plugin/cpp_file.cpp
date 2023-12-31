#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/compiler/code_generator.h>

#include "cpp_file.h"
#include "plugin_helper.h"

//
// CppFile
//
CppFile::CppFile(std::string_view file_name) :
    proto_file_name_(file_name),
    header_file_name_(file_name),
    source_file_name_(file_name)
{
    proto_file_name_ += ".proto";
    header_file_name_ += ".mrpc.h";
    source_file_name_ += ".mrpc.cpp";
}

bool CppFile::Parse(const google::protobuf::FileDescriptor* file_desc, std::string* error)
{
    // extensions
    gen_cpp_reflection_ = file_desc->options().GetExtension(mrpc::gen_cpp_reflection);

    // includes
    for (int i = 0; i < file_desc->dependency_count(); ++i)
    {
        const google::protobuf::FileDescriptor* desc = file_desc->dependency(i);
        const std::string& proto_file_name = desc->name();
        if (IsImportIgnored(proto_file_name)) continue;

        const size_t pos = proto_file_name.find_last_of('.');
        if (pos == std::string::npos)
        {
            *error = "Invalid import file name: " + proto_file_name;
            return false;
        }

        std::string include = proto_file_name.substr(0, pos);
        include += ".mrpc.h";
        includes_.push_back(include);
    }

    // namespace
    namespace_ = DotNameToCppTypeName(file_desc->package());

    // enums
    for (int i = 0; i < file_desc->enum_type_count(); ++i)
    {
        const google::protobuf::EnumDescriptor* desc = file_desc->enum_type(i);
        CppEnum& e = enums_.emplace_back(namespace_, desc->name());
        e.SetGenCppReflection(gen_cpp_reflection_);

        if (!e.Parse(desc, error))
        {
            return false;
        }
    }

    // classes
    for (int i = 0; i < file_desc->message_type_count(); ++i)
    {
        const google::protobuf::Descriptor* desc = file_desc->message_type(i);
        if (IsClassIgnored(desc->full_name())) continue;

        CppClass& clazz = classes_.emplace_back(namespace_, desc->name());
        clazz.SetGenCppReflection(gen_cpp_reflection_);

        if (!clazz.Parse(desc, error))
        {
            return false;
        }
    }

    return true;
}

void CppFile::OutputToHeaderFile(google::protobuf::compiler::GeneratorContext* ctx) const
{
    google::protobuf::io::Printer printer(ctx->Open(header_file_name_), '$');
    std::map<std::string, std::string> vars;

    // include macro
    vars["proto_file_name"] = proto_file_name_;
    // vars["include_macro"] = MakeIncludeMarco();
    printer.Print(vars,
            "// Generated by the protocol buffer compiler mrpc plugin. DO NOT EDIT!\n"
            "// source: $proto_file_name$\n"
            "\n"
            /*
            "#ifndef $include_macro$\n"
            "#define $include_macro$\n"
            */
            "#pragma once\n"
            "\n");

    // includes
    if (NeedIncludeCppTypeHeader(mrpc::CPPTYPE_STRING))
    {
        printer.Print("#include <string>\n");
    }
    if (NeedIncludeCppTypeHeader(mrpc::CPPTYPE_VECTOR))
    {
        printer.Print("#include <vector>\n");
    }
    if (NeedIncludeCppTypeHeader(mrpc::CPPTYPE_LIST))
    {
        printer.Print("#include <list>\n");
    }
    if (NeedIncludeCppTypeHeader(mrpc::CPPTYPE_MAP))
    {
        printer.Print("#include <map>\n");
    }
    if (NeedIncludeCppTypeHeader(mrpc::CPPTYPE_UNORDERED_MAP))
    {
        printer.Print("#include <unordered_map>\n");
    }
    printer.Print("\n");

    printer.Print("#include <mrpc/message/message.h>\n");
    /*
    if (gen_cpp_reflection_ && (!enums_.empty() || !classes_.empty()))
    {
        printer.Print("#include <mrpc/message/descriptor.h>\n");
    }
    */
    printer.Print("\n");

    for (auto& include : includes_)
    {
        vars["include"] = include;
        printer.Print(vars, "#include \"$include$\"\n");
    }
    printer.Print("\n");

    // namespace
    vars["namespace"] = namespace_;
    printer.Print(vars, "namespace $namespace$\n"
            "{\n"
            "\n");

    // enums
    for (auto& e : enums_)
    {
        e.OutputToHeaderFile(printer, vars);
    }

    // classes
    for (auto& clazz : classes_)
    {
        clazz.OutputToHeaderFile(printer, vars);
    }

    // namespace
    printer.Print("};\n"
            "\n");

    // include macro
    // printer.Print(vars, "#endif //$include_macro$");
}

void CppFile::OutputToSourceFile(google::protobuf::compiler::GeneratorContext* ctx) const
{
    google::protobuf::io::Printer printer(ctx->Open(source_file_name_), '$');
    std::map<std::string, std::string> vars;

    // includes
    vars["proto_file_name"] = proto_file_name_;
    vars["include"] = header_file_name_;
    printer.Print(vars,
            "// Generated by the protocol buffer compiler mrpc plugin. DO NOT EDIT!\n"
            "// source: $proto_file_name$\n"
            "\n"
            "#include <mrpc/message/message_internal.h>\n");

    if (gen_cpp_reflection_ && (!enums_.empty() || !classes_.empty()))
    {
        printer.Print("#include <mrpc/message/descriptor_internal.h>\n");
    }
    printer.Print(vars,
            "\n"
            "#include \"$include$\"\n"
            "\n");

    // enums
    for (auto& e : enums_)
    {
        e.OutputToSourceFile(printer, vars);
    }

    // classes
    for (auto& clazz : classes_)
    {
        clazz.OutputToSourceFile(printer, vars);
    }
}

std::string CppFile::MakeIncludeMarco() const
{
    std::string macro = header_file_name_;
    for (auto& c : macro)
    {
        if (c == '.')
        {
            c = '_';
        }
        else if (c >= 'a' && c <= 'z')
        {
            c = c - 'a' + 'A';
        }
    }
    return "__" + macro + "__";
}

bool CppFile::NeedIncludeCppTypeHeader(mrpc::CppType cpp_type) const
{
    for (auto& clazz : classes_)
    {
        if (clazz.HasCppTypeField(cpp_type)) return true;
    }
    return false;
}
