#include <cassert>
#include <google/protobuf/io/printer.h>

#include <mrpc/options.pb.h>

#include "cpp_field.h"
#include "plugin_helper.h"

//
// CppField
//
static_assert((int32_t)mrpc::CPPTYPE_VECTOR == (int32_t)mrpc::vector);
static_assert((int32_t)mrpc::CPPTYPE_LIST == (int32_t)mrpc::list);
static_assert((int32_t)mrpc::CPPTYPE_MAP == (int32_t)mrpc::map);
static_assert((int32_t)mrpc::CPPTYPE_UNORDERED_MAP == (int32_t)mrpc::unordered_map);

static const mrpc::CppType kPbCppTypeToMrpcCppType[google::protobuf::FieldDescriptor::MAX_CPPTYPE + 1] =
{
    mrpc::CPPTYPE_UNKNOWN,
    mrpc::CPPTYPE_INT32,
    mrpc::CPPTYPE_INT64,
    mrpc::CPPTYPE_UINT32,
    mrpc::CPPTYPE_UINT64,
    mrpc::CPPTYPE_DOUBLE,
    mrpc::CPPTYPE_FLOAT,
    mrpc::CPPTYPE_BOOL,
    mrpc::CPPTYPE_ENUM,
    mrpc::CPPTYPE_STRING,
    mrpc::CPPTYPE_MESSAGE
};

static const std::map<mrpc::CppType, std::string_view> kCppTypeToEnumName = 
{
    { mrpc::CPPTYPE_INT32, "mrpc::CPPTYPE_INT32" },
    { mrpc::CPPTYPE_UINT32, "mrpc::CPPTYPE_UINT32" },
    { mrpc::CPPTYPE_INT64, "mrpc::CPPTYPE_INT64" },
    { mrpc::CPPTYPE_UINT64, "mrpc::CPPTYPE_UINT64" },
    { mrpc::CPPTYPE_FLOAT, "mrpc::CPPTYPE_FLOAT" },
    { mrpc::CPPTYPE_DOUBLE, "mrpc::CPPTYPE_DOUBLE" },
    { mrpc::CPPTYPE_BOOL, "mrpc::CPPTYPE_BOOL" },
    { mrpc::CPPTYPE_ENUM, "mrpc::CPPTYPE_ENUM" },
    { mrpc::CPPTYPE_STRING, "mrpc::CPPTYPE_STRING" },
    { mrpc::CPPTYPE_MESSAGE, "mrpc::CPPTYPE_MESSAGE" },
    { mrpc::CPPTYPE_VECTOR, "mrpc::CPPTYPE_VECTOR" },
    { mrpc::CPPTYPE_LIST, "mrpc::CPPTYPE_LIST" },
    { mrpc::CPPTYPE_MAP, "mrpc::CPPTYPE_MAP" },
    { mrpc::CPPTYPE_UNORDERED_MAP, "mrpc::CPPTYPE_UNORDERED_MAP" }
};

static const std::map<mrpc::CppType, std::string_view> kCppTypeToName =
{
    { mrpc::CPPTYPE_INT32, "int32_t" },
    { mrpc::CPPTYPE_UINT32, "uint32_t" },
    { mrpc::CPPTYPE_INT64, "int64_t" },
    { mrpc::CPPTYPE_UINT64, "uint64_t" },
    { mrpc::CPPTYPE_FLOAT, "float" },
    { mrpc::CPPTYPE_DOUBLE, "double" },
    { mrpc::CPPTYPE_BOOL, "bool" },
    { mrpc::CPPTYPE_ENUM, "int32_t" },
    { mrpc::CPPTYPE_STRING, "std::string" },
    { mrpc::CPPTYPE_VECTOR, "std::vector" },
    { mrpc::CPPTYPE_LIST, "std::list" },
    { mrpc::CPPTYPE_MAP, "std::map" },
    { mrpc::CPPTYPE_UNORDERED_MAP, "std::unordered_map" }
};

static const std::map<mrpc::CppType, std::string_view> kCppTypeDefault =
{
    { mrpc::CPPTYPE_INT32, "0" },
    { mrpc::CPPTYPE_UINT32, "0" },
    { mrpc::CPPTYPE_INT64, "0l" },
    { mrpc::CPPTYPE_UINT64, "0ul" },
    { mrpc::CPPTYPE_FLOAT, "0.0f" },
    { mrpc::CPPTYPE_DOUBLE, "0.0" },
    { mrpc::CPPTYPE_BOOL, "false" }
};

static const std::map<int, std::string_view> kPbTypeToTemplateType =
{
    { google::protobuf::FieldDescriptor::TYPE_DOUBLE, "mrpc::TYPE_DOUBLE" },
    { google::protobuf::FieldDescriptor::TYPE_FLOAT, "mrpc::TYPE_FLOAT" },
    { google::protobuf::FieldDescriptor::TYPE_INT64, "mrpc::TYPE_VAR_INT64" },
    { google::protobuf::FieldDescriptor::TYPE_UINT64, "mrpc::TYPE_VAR_UINT64" },
    { google::protobuf::FieldDescriptor::TYPE_INT32, "mrpc::TYPE_VAR_INT32" },
    { google::protobuf::FieldDescriptor::TYPE_FIXED64, "mrpc::TYPE_FIXED_UINT64" },
    { google::protobuf::FieldDescriptor::TYPE_FIXED32, "mrpc::TYPE_FIXED_UINT32" },
    { google::protobuf::FieldDescriptor::TYPE_BOOL, "mrpc::TYPE_BOOL" },
    { google::protobuf::FieldDescriptor::TYPE_STRING, "mrpc::TYPE_STRING" },
    { google::protobuf::FieldDescriptor::TYPE_BYTES, "mrpc::TYPE_STRING" },
    { google::protobuf::FieldDescriptor::TYPE_UINT32, "mrpc::TYPE_VAR_UINT32" },
    { google::protobuf::FieldDescriptor::TYPE_ENUM, "mrpc::TYPE_VAR_INT32" },
    { google::protobuf::FieldDescriptor::TYPE_SFIXED32, "mrpc::TYPE_FIXED_INT32" },
    { google::protobuf::FieldDescriptor::TYPE_SFIXED64, "mrpc::TYPE_FIXED_INT64" },
    { google::protobuf::FieldDescriptor::TYPE_SINT32, "mrpc::TYPE_ZIGZAG_INT32" },
    { google::protobuf::FieldDescriptor::TYPE_SINT64, "mrpc::TYPE_ZIGZAG_INT64" },
};

static std::string_view CppTypeToEnumName(mrpc::CppType cpp_type)
{
    auto it = kCppTypeToEnumName.find(cpp_type);
    if (it != kCppTypeToEnumName.end())
    {
        return it->second;
    }
    return "";
}

static std::string_view CppTypeToName(mrpc::CppType cpp_type)
{
    auto it = kCppTypeToName.find(cpp_type);
    if (it != kCppTypeToName.end())
    {
        return it->second;
    }
    return "";
}

static std::string_view CppTypeDefault(mrpc::CppType cpp_type)
{
    auto it = kCppTypeDefault.find(cpp_type);
    if (it != kCppTypeDefault.end())
    {
        return it->second;
    }
    return "";
}


CppField::CppField(const std::string& field_name) : field_name_(field_name)
{
}

bool CppField::Parse(const google::protobuf::FieldDescriptor* desc, std::string* error)
{
    std::string field_type_full_name;
    tag_number_ = desc->number();
    proto_type_ = desc->type();
    cpp_type_ = kPbCppTypeToMrpcCppType[desc->cpp_type()];

    if (desc->is_packable() && !desc->is_packed())
    {
        *error = desc->full_name() + " must have option `[packed = true]`";
        return false;
    }

    // map
    if (desc->is_map())
    {
        const google::protobuf::Descriptor* map_desc = desc->message_type();
        proto_sub_type_1_ = map_desc->map_key()->type();
        proto_sub_type_2_ = map_desc->map_value()->type();

        cpp_type_ = mrpc::CPPTYPE_MAP;
        cpp_sub_type_1_ = kPbCppTypeToMrpcCppType[map_desc->map_key()->cpp_type()];
        cpp_sub_type_2_ = kPbCppTypeToMrpcCppType[map_desc->map_value()->cpp_type()];
        if (cpp_sub_type_2_ == mrpc::CPPTYPE_ENUM)
        {
            field_type_name_ = map_desc->map_value()->enum_type()->name();
            field_type_full_name = map_desc->map_value()->enum_type()->full_name();
        }
        else if (cpp_sub_type_2_ == mrpc::CPPTYPE_MESSAGE)
        {
            field_type_name_ = map_desc->map_value()->message_type()->name();
            field_type_full_name = map_desc->map_value()->message_type()->full_name();
        }
    }

    // enum/message
    if (cpp_type_ == mrpc::CPPTYPE_ENUM)
    {
        field_type_name_ = desc->enum_type()->name();
        field_type_full_name = desc->enum_type()->full_name();
    }
    else if (cpp_type_ == mrpc::CPPTYPE_MESSAGE)
    {
        field_type_name_ = desc->message_type()->name();
        field_type_full_name = desc->message_type()->full_name();
    }

    if (!field_type_name_.empty())
    {
        // different namespace
        if (desc->file()->package() + "." + field_type_name_ != field_type_full_name)
        {
            field_type_name_ = DotNameToCppTypeName(field_type_full_name);
        }
    }

    // repeated 
    if (!desc->is_map() && desc->is_repeated())
    {
        proto_sub_type_1_ = proto_type_;
        cpp_sub_type_1_ = cpp_type_;
        cpp_type_ = mrpc::CPPTYPE_VECTOR;
    }

    // container type
    if (desc->options().HasExtension(mrpc::cpp_type))
    {
        if (!IsContainerType(cpp_type_))
        {
            *error = desc->full_name() + ": only repeated fields and maps can set `mrpc.cpp_type`";
            return false;
        }

        mrpc::CppType real_cpp_type = (mrpc::CppType)desc->options().GetExtension(mrpc::cpp_type);
        if (IsSequenceContainerType(cpp_type_))
        {
            if (!IsSequenceContainerType(real_cpp_type))
            {
                *error = desc->full_name() + ": repeated fields can only set `mrpc.cpp_type` to `vector` or `list`";
                return false;
            }
        }
        else if (IsAssociativeContainerType(cpp_type_))
        {
            if (!IsAssociativeContainerType(real_cpp_type))
            {
                *error = desc->full_name() + ": maps can only set `mrpc.cpp_type` to `map` or `unordered_map`";
                return false;
            }
        }

        cpp_type_ = real_cpp_type;
    }

    if (cpp_type_ == mrpc::CPPTYPE_VECTOR && cpp_sub_type_1_ == mrpc::CPPTYPE_BOOL)
    {
        *error = desc->full_name() + ": repeated bool fields must not set `mrpc.cpp_type` to `vector`";
        return false;
    }

    // default value
    switch (cpp_type_)
    {
        case mrpc::CPPTYPE_INT32:
            field_default_ = std::to_string(desc->default_value_int32());
            break;
        case mrpc::CPPTYPE_UINT32:
            field_default_ = std::to_string(desc->default_value_uint32());
            break;
        case mrpc::CPPTYPE_INT64:
            field_default_ = std::to_string(desc->default_value_int64()) + "l";
            break;
        case mrpc::CPPTYPE_UINT64:
            field_default_ = std::to_string(desc->default_value_uint64()) + "ul";
            break;
        case mrpc::CPPTYPE_FLOAT:
            if (desc->has_default_value())
            {
                field_default_ = std::to_string(desc->default_value_float()) + "f";
            }
            break;
        case mrpc::CPPTYPE_DOUBLE:
            if (desc->has_default_value())
            {
                field_default_ = std::to_string(desc->default_value_double());
            }
            break;
        case mrpc::CPPTYPE_BOOL:
            if (desc->default_value_bool())
            {
                field_default_ = "true";
            }
            break;
        case mrpc::CPPTYPE_ENUM:
            field_default_ = desc->default_value_enum()->name();
            // different namespace
            if (desc->file()->package() + "." + field_default_ != desc->default_value_enum()->full_name())
            {
                field_default_ = DotNameToCppTypeName(desc->default_value_enum()->full_name());
            }
            break;
        case mrpc::CPPTYPE_STRING:
            if (desc->has_default_value())
            {
                field_default_ = desc->default_value_string();
            }
            break;
        default:
            break;
    }

    return true;
}

void CppField::OutputFieldDefinition(google::protobuf::io::Printer& printer,
        std::map<std::string, std::string>& vars) const
{
    // field
    vars["field_name"] = field_name_;
    vars["field_type"] = CppTypeToName(cpp_type_);
    vars["field_sub_type_1"] = CppTypeToName(cpp_sub_type_1_);
    vars["field_sub_type_2"] = CppTypeToName(cpp_sub_type_2_);
    if (!field_default_.empty())
    {
        vars["field_default"] = field_default_;
    }
    else
    {
        vars["field_default"] = CppTypeDefault(cpp_type_);
    }
    switch (cpp_type_)
    {
        case mrpc::CPPTYPE_INT32:
        case mrpc::CPPTYPE_UINT32:
        case mrpc::CPPTYPE_INT64:
        case mrpc::CPPTYPE_UINT64:
        case mrpc::CPPTYPE_FLOAT:
        case mrpc::CPPTYPE_DOUBLE:
        case mrpc::CPPTYPE_BOOL:
            printer.Print(vars, "    $field_type$ $field_name$ = $field_default$;\n");
            break;
        case mrpc::CPPTYPE_ENUM:
            // enum类型为int32_t
            // vars["field_type"] = field_type_name_;
            printer.Print(vars, "    $field_type$ $field_name$ = $field_default$;\n");
            break;
        case mrpc::CPPTYPE_STRING:
            if (!vars["field_default"].empty())
            {
                printer.Print(vars, "    $field_type$ $field_name$ = \"$field_default$\";\n");
            }
            else
            {
                printer.Print(vars, "    $field_type$ $field_name$;\n");
            }
            break;
        case mrpc::CPPTYPE_VECTOR:
        case mrpc::CPPTYPE_LIST:
            if (IsNamedType(cpp_sub_type_1_))
            {
                vars["field_sub_type_1"] = field_type_name_;
            }
            printer.Print(vars, "    $field_type$<$field_sub_type_1$> $field_name$;\n");
            break;
        case mrpc::CPPTYPE_MAP:
        case mrpc::CPPTYPE_UNORDERED_MAP:
            if (IsNamedType(cpp_sub_type_2_))
            {
                vars["field_sub_type_2"] = field_type_name_;
            }
            printer.Print(vars, "    $field_type$<$field_sub_type_1$, $field_sub_type_2$> $field_name$;\n");
            break;
        case mrpc::CPPTYPE_MESSAGE:
            vars["field_type"] = field_type_name_;
            printer.Print(vars, "    $field_type$ $field_name$;\n");
            break;
        default:
            assert(false && "unknown type");
            break;
    }
}

void CppField::OutputFieldCacheSize(google::protobuf::io::Printer& printer,
        std::map<std::string, std::string>& vars) const
{
    if (!IsSequenceContainerType(cpp_type_)) return;
    if (cpp_sub_type_1_ == mrpc::CPPTYPE_STRING || cpp_sub_type_1_ == mrpc::CPPTYPE_MESSAGE) return;

    // field
    vars["field_name"] = field_name_;
    printer.Print(vars, "    mutable uint32_t $field_name$_cached_size_ = 0;\n");
}

void CppField::OutputClearMethod(google::protobuf::io::Printer& printer,
        std::map<std::string, std::string>& vars) const
{
    vars["field_name"] = field_name_;
    if (!field_default_.empty())
    {
        vars["field_default"] = field_default_;
    }
    else
    {
        vars["field_default"] = CppTypeDefault(cpp_type_);
    }
    switch (cpp_type_)
    {
        case mrpc::CPPTYPE_INT32:
        case mrpc::CPPTYPE_UINT32:
        case mrpc::CPPTYPE_INT64:
        case mrpc::CPPTYPE_UINT64:
        case mrpc::CPPTYPE_FLOAT:
        case mrpc::CPPTYPE_DOUBLE:
        case mrpc::CPPTYPE_BOOL:
        case mrpc::CPPTYPE_ENUM:
            printer.Print(vars, "    $field_name$ = $field_default$;\n");
            break;
        case mrpc::CPPTYPE_STRING:
            if (!vars["field_default"].empty())
            {
                printer.Print(vars, "    $field_name$ = \"$field_default$\";\n");
            }
            else
            {
                printer.Print(vars, "    $field_name$.clear();\n");
            }
            break;
        case mrpc::CPPTYPE_VECTOR:
        case mrpc::CPPTYPE_LIST:
        case mrpc::CPPTYPE_MAP:
        case mrpc::CPPTYPE_UNORDERED_MAP:
            printer.Print(vars, "    $field_name$.clear();\n");
            break;
        case mrpc::CPPTYPE_MESSAGE:
            printer.Print(vars, "    $field_name$.Clear();\n");
            break;
        default:
            assert(false && "unknown type");
            break;
    }
}

void CppField::OutputByteSizeSkipDefaultMethod(google::protobuf::io::Printer& printer,
        std::map<std::string, std::string>& vars) const
{
    // field
    vars["field_name"] = field_name_;
    vars["tag_number"] = std::to_string(tag_number_);
    vars["skip_default"] = "true";
    if (!field_default_.empty())
    {
        vars["field_default"] = field_default_;
    }
    else
    {
        vars["field_default"] = CppTypeDefault(cpp_type_);
    }

    if (!IsContainerType(cpp_type_))
    {
        if (cpp_type_ == mrpc::CPPTYPE_MESSAGE)
        {
            printer.Print(vars, "    size += mrpc::CalcByteSizeWithTag<$skip_default$>($tag_number$, this->$field_name$);\n");
        }
        else
        {
            auto it = kPbTypeToTemplateType.find(proto_type_);
            if (it != kPbTypeToTemplateType.end())
            {
                vars["template_type"] = it->second;
            }
            else
            {
                assert(false && "unknown type");
            }

            switch (cpp_type_)
            {
                case mrpc::CPPTYPE_INT32:
                case mrpc::CPPTYPE_UINT32:
                case mrpc::CPPTYPE_INT64:
                case mrpc::CPPTYPE_UINT64:
                case mrpc::CPPTYPE_FLOAT:
                case mrpc::CPPTYPE_DOUBLE:
                case mrpc::CPPTYPE_ENUM:
                    printer.Print(vars, "    if (this->$field_name$ != $field_default$) size += mrpc::CalcByteSizeWithTag<$template_type$>($tag_number$, this->$field_name$);\n");
                    break;
                case mrpc::CPPTYPE_BOOL:
                    if (vars["field_default"] == "false")
                    {
                        printer.Print(vars, "    if (this->$field_name$) size += mrpc::CalcByteSizeWithTag<$template_type$>($tag_number$, this->$field_name$);\n");
                    }
                    else
                    {
                        printer.Print(vars, "    if (!this->$field_name$) size += mrpc::CalcByteSizeWithTag<$template_type$>($tag_number$, this->$field_name$);\n");
                    }
                    break;
                case mrpc::CPPTYPE_STRING:
                    if (!vars["field_default"].empty())
                    {
                        printer.Print(vars, "    if (this->$field_name$ != \"$field_default$\") size += mrpc::CalcByteSizeWithTag<$template_type$>($tag_number$, this->$field_name$);\n");
                    }
                    else
                    {
                        printer.Print(vars, "    if (!this->$field_name$.empty()) size += mrpc::CalcByteSizeWithTag<$template_type$>($tag_number$, this->$field_name$);\n");
                    }
                    break;
                default:
                    break;
            }
        }
    }
    else if (IsSequenceContainerType(cpp_type_))
    {
        if (cpp_sub_type_1_ == mrpc::CPPTYPE_STRING)
        {
            printer.Print(vars, "    size += mrpc::CalcRepeatedByteSizeWithTag($tag_number$, this->$field_name$);\n");
        }
        else if (cpp_sub_type_1_ == mrpc::CPPTYPE_MESSAGE)
        {
            printer.Print(vars, "    size += mrpc::CalcRepeatedByteSizeWithTag<$skip_default$>($tag_number$, this->$field_name$);\n");
        }
        else
        {
            auto it = kPbTypeToTemplateType.find(proto_sub_type_1_);
            if (it != kPbTypeToTemplateType.end())
            {
                vars["template_type"] = it->second;
            }
            else
            {
                assert(false && "unknown type");
            }

            printer.Print(vars, "    size += mrpc::CalcRepeatedByteSizeWithTag<$template_type$>($tag_number$, this->$field_name$, $field_name$_cached_size_);\n");
        }
    }
    else if (IsAssociativeContainerType(cpp_type_))
    {
        if (cpp_sub_type_2_ == mrpc::CPPTYPE_MESSAGE)
        {
            auto it_key = kPbTypeToTemplateType.find(proto_sub_type_1_);
            if (it_key != kPbTypeToTemplateType.end())
            {
                vars["template_type_key"] = it_key->second;
            }
            else
            {
                assert(false && "unknown type");
            }

            printer.Print(vars, "    size += mrpc::CalcMapByteSizeWithTag<$template_type_key$, $skip_default$>($tag_number$, this->$field_name$);\n");
        }
        else
        {
            auto it_key = kPbTypeToTemplateType.find(proto_sub_type_1_);
            auto it_value = kPbTypeToTemplateType.find(proto_sub_type_2_);
            if (it_key != kPbTypeToTemplateType.end() && it_value != kPbTypeToTemplateType.end())
            {
                vars["template_type_key"] = it_key->second;
                vars["template_type_value"] = it_value->second;
            }
            else
            {
                assert(false && "unknown type");
            }

            printer.Print(vars, "    size += mrpc::CalcMapByteSizeWithTag<$template_type_key$, $template_type_value$>($tag_number$, this->$field_name$);\n");
        }
    }
}

void CppField::OutputByteSizeNotSkipDefaultMethod(google::protobuf::io::Printer& printer,
        std::map<std::string, std::string>& vars) const
{
    vars["field_name"] = field_name_;
    vars["tag_number"] = std::to_string(tag_number_);
    vars["skip_default"] = "false";

    if (!IsContainerType(cpp_type_))
    {
        if (cpp_type_ == mrpc::CPPTYPE_MESSAGE)
        {
            printer.Print(vars, "    size += mrpc::CalcByteSizeWithTag<$skip_default$>($tag_number$, this->$field_name$);\n");
        }
        else
        {
            auto it = kPbTypeToTemplateType.find(proto_type_);
            if (it != kPbTypeToTemplateType.end())
            {
                vars["template_type"] = it->second;
            }
            else
            {
                assert(false && "unknown type");
            }

            printer.Print(vars, "    size += mrpc::CalcByteSizeWithTag<$template_type$>($tag_number$, this->$field_name$);\n");
        }

    }
    else if (IsSequenceContainerType(cpp_type_))
    {
        if (cpp_sub_type_1_ == mrpc::CPPTYPE_STRING)
        {
            printer.Print(vars, "    size += mrpc::CalcRepeatedByteSizeWithTag($tag_number$, this->$field_name$);\n");
        }
        else if (cpp_sub_type_1_ == mrpc::CPPTYPE_MESSAGE)
        {
            printer.Print(vars, "    size += mrpc::CalcRepeatedByteSizeWithTag<$skip_default$>($tag_number$, this->$field_name$);\n");
        }
        else
        {
            auto it = kPbTypeToTemplateType.find(proto_sub_type_1_);
            if (it != kPbTypeToTemplateType.end())
            {
                vars["template_type"] = it->second;
            }
            else
            {
                assert(false && "unknown type");
            }

            printer.Print(vars, "    size += mrpc::CalcRepeatedByteSizeWithTag<$template_type$>($tag_number$, this->$field_name$, $field_name$_cached_size_);\n");
        }
    }
    else if (IsAssociativeContainerType(cpp_type_))
    {
        if (cpp_sub_type_2_ == mrpc::CPPTYPE_MESSAGE)
        {
            auto it_key = kPbTypeToTemplateType.find(proto_sub_type_1_);
            if (it_key != kPbTypeToTemplateType.end())
            {
                vars["template_type_key"] = it_key->second;
            }
            else
            {
                assert(false && "unknown type");
            }

            printer.Print(vars, "    size += mrpc::CalcMapByteSizeWithTag<$template_type_key$, $skip_default$>($tag_number$, this->$field_name$);\n");
        }
        else
        {
            auto it_key = kPbTypeToTemplateType.find(proto_sub_type_1_);
            auto it_value = kPbTypeToTemplateType.find(proto_sub_type_2_);
            if (it_key != kPbTypeToTemplateType.end() && it_value != kPbTypeToTemplateType.end())
            {
                vars["template_type_key"] = it_key->second;
                vars["template_type_value"] = it_value->second;
            }
            else
            {
                assert(false && "unknown type");
            }

            printer.Print(vars, "    size += mrpc::CalcMapByteSizeWithTag<$template_type_key$, $template_type_value$>($tag_number$, this->$field_name$);\n");
        }
    }
}

void CppField::OutputSerializeToStringSkipDefaultMethod(google::protobuf::io::Printer& printer,
        std::map<std::string, std::string>& vars) const
{
    vars["field_name"] = field_name_;
    vars["tag_number"] = std::to_string(tag_number_);
    vars["skip_default"] = "true";
    if (!field_default_.empty())
    {
        vars["field_default"] = field_default_;
    }
    else
    {
        vars["field_default"] = CppTypeDefault(cpp_type_);
    }

    if (!IsContainerType(cpp_type_))
    {
        if (cpp_type_ == mrpc::CPPTYPE_MESSAGE)
        {
            printer.Print(vars, "    mrpc::SerializeWithTag<$skip_default$>(s, $tag_number$, this->$field_name$);\n");
        }
        else
        {
            auto it = kPbTypeToTemplateType.find(proto_type_);
            if (it != kPbTypeToTemplateType.end())
            {
                vars["template_type"] = it->second;
            }
            else
            {
                assert(false && "unknown type");
            }

            switch (cpp_type_)
            {
                case mrpc::CPPTYPE_INT32:
                case mrpc::CPPTYPE_UINT32:
                case mrpc::CPPTYPE_INT64:
                case mrpc::CPPTYPE_UINT64:
                case mrpc::CPPTYPE_FLOAT:
                case mrpc::CPPTYPE_DOUBLE:
                case mrpc::CPPTYPE_ENUM:
                    printer.Print(vars, "    if (this->$field_name$ != $field_default$) mrpc::SerializeWithTag<$template_type$>(s, $tag_number$, this->$field_name$);\n");
                    break;
                case mrpc::CPPTYPE_BOOL:
                    if (vars["field_default"] == "false")
                    {
                        printer.Print(vars, "    if (this->$field_name$) mrpc::SerializeWithTag<$template_type$>(s, $tag_number$, this->$field_name$);\n");
                    }
                    else
                    {
                        printer.Print(vars, "    if (!this->$field_name$) mrpc::SerializeWithTag<$template_type$>(s, $tag_number$, this->$field_name$);\n");
                    }
                    break;
                case mrpc::CPPTYPE_STRING:
                    if (!vars["field_default"].empty())
                    {
                        printer.Print(vars, "    if (this->$field_name$ != \"$field_default$\") mrpc::SerializeWithTag<$template_type$>(s, $tag_number$, this->$field_name$);\n");
                    }
                    else
                    {
                        printer.Print(vars, "    if (!this->$field_name$.empty()) mrpc::SerializeWithTag<$template_type$>(s, $tag_number$, this->$field_name$);\n");
                    }
                    break;
                default:
                    break;
            }
        }

    }
    else if (IsSequenceContainerType(cpp_type_))
    {
        if (cpp_sub_type_1_ == mrpc::CPPTYPE_STRING)
        {
            printer.Print(vars, "    mrpc::SerializeRepeatedWithTag(s, $tag_number$, this->$field_name$);\n");
        }
        else if (cpp_sub_type_1_ == mrpc::CPPTYPE_MESSAGE)
        {
            printer.Print(vars, "    mrpc::SerializeRepeatedWithTag<$skip_default$>(s, $tag_number$, this->$field_name$);\n");
        }
        else
        {
            auto it = kPbTypeToTemplateType.find(proto_sub_type_1_);
            if (it != kPbTypeToTemplateType.end())
            {
                vars["template_type"] = it->second;
            }
            else
            {
                assert(false && "unknown type");
            }

            printer.Print(vars, "    mrpc::SerializeRepeatedWithTag<$template_type$>(s, $tag_number$, this->$field_name$, $field_name$_cached_size_);\n");
        }
    }
    else if (IsAssociativeContainerType(cpp_type_))
    {
        if (cpp_sub_type_2_ == mrpc::CPPTYPE_MESSAGE)
        {
            auto it_key = kPbTypeToTemplateType.find(proto_sub_type_1_);
            if (it_key != kPbTypeToTemplateType.end())
            {
                vars["template_type_key"] = it_key->second;
            }
            else
            {
                assert(false && "unknown type");
            }

            printer.Print(vars, "    mrpc::SerializeMapWithTag<$template_type_key$, $skip_default$>(s, $tag_number$, this->$field_name$);\n");
        }
        else
        {
            auto it_key = kPbTypeToTemplateType.find(proto_sub_type_1_);
            auto it_value = kPbTypeToTemplateType.find(proto_sub_type_2_);
            if (it_key != kPbTypeToTemplateType.end() && it_value != kPbTypeToTemplateType.end())
            {
                vars["template_type_key"] = it_key->second;
                vars["template_type_value"] = it_value->second;
            }
            else
            {
                assert(false && "unknown type");
            }

            printer.Print(vars, "    mrpc::SerializeMapWithTag<$template_type_key$, $template_type_value$>(s, $tag_number$, this->$field_name$);\n");
        }
    }
}

void CppField::OutputSerializeToStringNotSkipDefaultMethod(google::protobuf::io::Printer& printer,
        std::map<std::string, std::string>& vars) const
{
    vars["field_name"] = field_name_;
    vars["tag_number"] = std::to_string(tag_number_);
    vars["skip_default"] = "false";

    if (!IsContainerType(cpp_type_))
    {
        if (cpp_type_ == mrpc::CPPTYPE_MESSAGE)
        {
            printer.Print(vars, "    mrpc::SerializeWithTag<$skip_default$>(s, $tag_number$, this->$field_name$);\n");
        }
        else
        {
            auto it = kPbTypeToTemplateType.find(proto_type_);
            if (it != kPbTypeToTemplateType.end())
            {
                vars["template_type"] = it->second;
            }
            else
            {
                assert(false && "unknown type");
            }

            printer.Print(vars, "    mrpc::SerializeWithTag<$template_type$>(s, $tag_number$, this->$field_name$);\n");
        }
    }
    else if (IsSequenceContainerType(cpp_type_))
    {
        if (cpp_sub_type_1_ == mrpc::CPPTYPE_STRING)
        {
            printer.Print(vars, "    mrpc::SerializeRepeatedWithTag(s, $tag_number$, this->$field_name$);\n");
        }
        else if (cpp_sub_type_1_ == mrpc::CPPTYPE_MESSAGE)
        {
            printer.Print(vars, "    mrpc::SerializeRepeatedWithTag<$skip_default$>(s, $tag_number$, this->$field_name$);\n");
        }
        else
        {
            auto it = kPbTypeToTemplateType.find(proto_sub_type_1_);
            if (it != kPbTypeToTemplateType.end())
            {
                vars["template_type"] = it->second;
            }
            else
            {
                assert(false && "unknown type");
            }

            printer.Print(vars, "    mrpc::SerializeRepeatedWithTag<$template_type$>(s, $tag_number$, this->$field_name$, $field_name$_cached_size_);\n");
        }
    }
    else if (IsAssociativeContainerType(cpp_type_))
    {
        if (cpp_sub_type_2_ == mrpc::CPPTYPE_MESSAGE)
        {
            auto it_key = kPbTypeToTemplateType.find(proto_sub_type_1_);
            if (it_key != kPbTypeToTemplateType.end())
            {
                vars["template_type_key"] = it_key->second;
            }
            else
            {
                assert(false && "unknown type");
            }

            printer.Print(vars, "    mrpc::SerializeMapWithTag<$template_type_key$, $skip_default$>(s, $tag_number$, this->$field_name$);\n");
        }
        else
        {
            auto it_key = kPbTypeToTemplateType.find(proto_sub_type_1_);
            auto it_value = kPbTypeToTemplateType.find(proto_sub_type_2_);
            if (it_key != kPbTypeToTemplateType.end() && it_value != kPbTypeToTemplateType.end())
            {
                vars["template_type_key"] = it_key->second;
                vars["template_type_value"] = it_value->second;
            }
            else
            {
                assert(false && "unknown type");
            }

            printer.Print(vars, "    mrpc::SerializeMapWithTag<$template_type_key$, $template_type_value$>(s, $tag_number$, this->$field_name$);\n");
        }
    }
}

void CppField::OutputParseFromBytesMethod(google::protobuf::io::Printer& printer,
        std::map<std::string, std::string>& vars) const
{
    vars["field_name"] = field_name_;
    vars["tag_number"] = std::to_string(tag_number_);
    if (!IsContainerType(cpp_type_))
    {
        if (cpp_type_ == mrpc::CPPTYPE_MESSAGE)
        {
            printer.Print(vars,
                    "            case $tag_number$: if (!mrpc::ParseCheckType(type, this->$field_name$, begin, end)) return false;\n"
                    "                break;\n");
        }
        /*
        else if (cpp_type_ == mrpc::CPPTYPE_ENUM)
        {
            auto it = kPbTypeToTemplateType.find(proto_type_);
            if (it != kPbTypeToTemplateType.end())
            {
                vars["template_type"] = it->second;
            }
            else
            {
                assert(false && "unknown type");
            }

            vars["field_type"] = field_type_name_;
            printer.Print(vars,
                    "            case $tag_number$:\n"
                    "            {\n"
                    "                int $field_name$_enum_value = 0;\n"
                    "                if (!mrpc::ParseCheckType<$template_type$>(type, $field_name$_enum_value, begin, end)) return false;\n"
                    "                if (!$field_type$_IsValid($field_name$_enum_value)) return false;\n"
                    "                this->$field_name$ = static_cast<$field_type$>($field_name$_enum_value);\n"
                    "                break;\n"
                    "            }\n");
        }
        */
        else
        {
            auto it = kPbTypeToTemplateType.find(proto_type_);
            if (it != kPbTypeToTemplateType.end())
            {
                vars["template_type"] = it->second;
            }
            else
            {
                assert(false && "unknown type");
            }

            printer.Print(vars,
                    "            case $tag_number$: if (!mrpc::ParseCheckType<$template_type$>(type, this->$field_name$, begin, end)) return false;\n"
                    "                break;\n");
        }
    }
    else if (IsSequenceContainerType(cpp_type_))
    {
        if (cpp_sub_type_1_ == mrpc::CPPTYPE_STRING)
        {
            printer.Print(vars,
                    "            case $tag_number$: if (!mrpc::ParseRepeatedCheckType(type, this->$field_name$, begin, end)) return false;\n"
                    "                break;\n");
        }
        else if (cpp_sub_type_1_ == mrpc::CPPTYPE_MESSAGE)
        {
            printer.Print(vars,
                    "            case $tag_number$: if (!mrpc::ParseRepeatedCheckType(type, this->$field_name$, begin, end)) return false;\n"
                    "                break;\n");
        }
        else
        {
            auto it = kPbTypeToTemplateType.find(proto_sub_type_1_);
            if (it != kPbTypeToTemplateType.end())
            {
                vars["template_type"] = it->second;
            }
            else
            {
                assert(false && "unknown type");
            }

            printer.Print(vars,
                    "            case $tag_number$: if (!mrpc::ParseRepeatedCheckType<$template_type$>(type, this->$field_name$, begin, end)) return false;\n"
                    "                break;\n");
        }
    }
    else if (IsAssociativeContainerType(cpp_type_))
    {
        if (cpp_sub_type_2_ == mrpc::CPPTYPE_MESSAGE)
        {
            auto it_key = kPbTypeToTemplateType.find(proto_sub_type_1_);
            if (it_key != kPbTypeToTemplateType.end())
            {
                vars["template_type_key"] = it_key->second;
            }
            else
            {
                assert(false && "unknown type");
            }

            printer.Print(vars,
                    "            case $tag_number$: if (!mrpc::ParseMapCheckType<$template_type_key$>(type, this->$field_name$, begin, end)) return false;\n"
                    "                break;\n");
        }
        else
        {
            auto it_key = kPbTypeToTemplateType.find(proto_sub_type_1_);
            auto it_value = kPbTypeToTemplateType.find(proto_sub_type_2_);
            if (it_key != kPbTypeToTemplateType.end() && it_value != kPbTypeToTemplateType.end())
            {
                vars["template_type_key"] = it_key->second;
                vars["template_type_value"] = it_value->second;
            }
            else
            {
                assert(false && "unknown type");
            }

            printer.Print(vars,
                    "            case $tag_number$: if (!mrpc::ParseMapCheckType<$template_type_key$, $template_type_value$>(type, this->$field_name$, begin, end)) return false;\n"
                    "                break;\n");
        }
    }
}

void CppField::OutputDescriptorWrapperMember(google::protobuf::io::Printer& printer,
        std::map<std::string, std::string>& vars) const
{
    vars["field_name"] = field_name_;
    vars["field_name_length"] = std::to_string(field_name_.length());
    vars["cpp_type_name"] = CppTypeToEnumName(cpp_type_);

    if (IsSequenceContainerType(cpp_type_))
    {
        vars["field_sub_type_1"] = CppTypeToEnumName(cpp_sub_type_1_);
        vars["container_sub_type_1"] = CppTypeToName(cpp_sub_type_1_);

        if (cpp_type_ == mrpc::CPPTYPE_VECTOR)
        {
            vars["container_impl_type"] = "Vector";
        }
        else if (cpp_type_ == mrpc::CPPTYPE_LIST)
        {
            vars["container_impl_type"] = "List";
        }

        if (cpp_sub_type_1_ == mrpc::CPPTYPE_ENUM)
        {
            vars["field_type"] = field_type_name_;
            printer.Print(vars, "static mrpc::$container_impl_type$FieldDescriptorImpl<$container_sub_type_1$> $field_name$_field_desc = { std::string_view(\"$field_name$\", $field_name_length$), $cpp_type_name$, mrpc::OffsetOf($class_name$, $field_name$), $field_sub_type_1$, $field_type$_GetDescriptor() };\n");
        }
        else if (cpp_sub_type_1_ == mrpc::CPPTYPE_MESSAGE)
        {
            vars["field_type"] = field_type_name_;
            printer.Print(vars, "static mrpc::$container_impl_type$FieldDescriptorImpl<$field_type$> $field_name$_field_desc = { std::string_view(\"$field_name$\", $field_name_length$), $cpp_type_name$, mrpc::OffsetOf($class_name$, $field_name$), $field_sub_type_1$, $field_type$::GetClassDescriptor() };\n");
        }
        else
        {
            printer.Print(vars, "static mrpc::$container_impl_type$FieldDescriptorImpl<$container_sub_type_1$> $field_name$_field_desc = { std::string_view(\"$field_name$\", $field_name_length$), $cpp_type_name$, mrpc::OffsetOf($class_name$, $field_name$), $field_sub_type_1$ };\n");
        }
    }
    else if (IsAssociativeContainerType(cpp_type_))
    {
        vars["field_sub_type_1"] = CppTypeToEnumName(cpp_sub_type_1_);
        vars["field_sub_type_2"] = CppTypeToEnumName(cpp_sub_type_2_);
        vars["container_sub_type_1"] = CppTypeToName(cpp_sub_type_1_);
        vars["container_sub_type_2"] = CppTypeToName(cpp_sub_type_2_);

        if (cpp_type_ == mrpc::CPPTYPE_MAP)
        {
            vars["container_impl_type"] = "Map";
        }
        else if (cpp_type_ == mrpc::CPPTYPE_UNORDERED_MAP)
        {
            vars["container_impl_type"] = "UnorderedMap";
        }

        if (cpp_sub_type_2_ == mrpc::CPPTYPE_ENUM)
        {
            vars["field_type"] = field_type_name_;
            printer.Print(vars, "static mrpc::$container_impl_type$FieldDescriptorImpl<$container_sub_type_1$, $container_sub_type_2$> $field_name$_field_desc = { std::string_view(\"$field_name$\", $field_name_length$), $cpp_type_name$, mrpc::OffsetOf($class_name$, $field_name$), $field_sub_type_1$, $field_sub_type_2$, $field_type$_GetDescriptor() };\n");
        }
        else if (cpp_sub_type_2_ == mrpc::CPPTYPE_MESSAGE)
        {
            vars["field_type"] = field_type_name_;
            printer.Print(vars, "static mrpc::$container_impl_type$FieldDescriptorImpl<$container_sub_type_1$, $field_type$> $field_name$_field_desc = { std::string_view(\"$field_name$\", $field_name_length$), $cpp_type_name$, mrpc::OffsetOf($class_name$, $field_name$), $field_sub_type_1$, $field_sub_type_2$, $field_type$::GetClassDescriptor() };\n");
        }
        else
        {
            printer.Print(vars, "static mrpc::$container_impl_type$FieldDescriptorImpl<$container_sub_type_1$, $container_sub_type_2$> $field_name$_field_desc = { std::string_view(\"$field_name$\", $field_name_length$), $cpp_type_name$, mrpc::OffsetOf($class_name$, $field_name$), $field_sub_type_1$, $field_sub_type_2$ };\n");
        }
    }
    else
    {
        if (cpp_type_ == mrpc::CPPTYPE_ENUM)
        {
            vars["field_type"] = field_type_name_;
            printer.Print(vars, "static mrpc::EnumFieldDescriptor $field_name$_field_desc = { std::string_view(\"$field_name$\", $field_name_length$), $cpp_type_name$, mrpc::OffsetOf($class_name$, $field_name$), $field_type$_GetDescriptor() };\n");
        }
        else if (cpp_type_ == mrpc::CPPTYPE_MESSAGE)
        {
            vars["field_type"] = field_type_name_;
            printer.Print(vars, "static mrpc::MessageFieldDescriptor $field_name$_field_desc = { std::string_view(\"$field_name$\", $field_name_length$), $cpp_type_name$, mrpc::OffsetOf($class_name$, $field_name$), $field_type$::GetClassDescriptor() };\n");
        }
        else
        {
            printer.Print(vars, "static mrpc::FieldDescriptor $field_name$_field_desc = { std::string_view(\"$field_name$\", $field_name_length$), $cpp_type_name$, mrpc::OffsetOf($class_name$, $field_name$) };\n");
        }
    }
}

void CppField::OutputDescriptorInitializerList(google::protobuf::io::Printer& printer,
        std::map<std::string, std::string>& vars) const
{
    vars["field_name"] = field_name_;
    vars["field_name_length"] = std::to_string(field_name_.length());
    vars["cpp_type_name"] = CppTypeToEnumName(cpp_type_);

    printer.Print(vars, "        &$field_name$_field_desc,\n");
}

bool CppField::IsNamedType(mrpc::CppType cpp_type)
{
    // enum类型为int32_t
    // return cpp_type == mrpc::CPPTYPE_ENUM || cpp_type == mrpc::CPPTYPE_MESSAGE;
    return cpp_type == mrpc::CPPTYPE_MESSAGE;
}

bool CppField::IsContainerType(mrpc::CppType cpp_type)
{
    return IsSequenceContainerType(cpp_type) || IsAssociativeContainerType(cpp_type);
}

bool CppField::IsSequenceContainerType(mrpc::CppType cpp_type)
{
    return cpp_type == mrpc::CPPTYPE_VECTOR || cpp_type == mrpc::CPPTYPE_LIST;
}

bool CppField::IsAssociativeContainerType(mrpc::CppType cpp_type)
{
    return cpp_type == mrpc::CPPTYPE_MAP || cpp_type == mrpc::CPPTYPE_UNORDERED_MAP;
}
