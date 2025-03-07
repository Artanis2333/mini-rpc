#include <mrpc/message/reflection.h>

namespace mrpc::reflection
{

int32_t GetEnum(const Message& msg, const FieldDescriptor& desc)
{
    assert(desc.GetCppType() == CPPTYPE_ENUM);
    return *reinterpret_cast<const int32_t*>(reinterpret_cast<const char*>(&msg) + desc.GetOffset());
}

bool SetEnum(Message& msg, const FieldDescriptor& desc, int32_t value)
{
    assert(desc.GetCppType() == CPPTYPE_ENUM);
    const EnumFieldDescriptor* field_desc = dynamic_cast<const EnumFieldDescriptor*>(&desc);
    assert(field_desc != nullptr);
    const EnumDescriptor* enum_desc = field_desc->GetEnumDescriptor();
    assert(enum_desc != nullptr);

    if (!enum_desc->IsValid(value))
    {
        return false;
    }
    *reinterpret_cast<int32_t*>(reinterpret_cast<char*>(&msg) + desc.GetOffset()) = value;
    return true;
}

std::string_view GetEnumName(const Message& msg, const FieldDescriptor& desc)
{
    assert(desc.GetCppType() == CPPTYPE_ENUM);
    int32_t value = *reinterpret_cast<const int32_t*>(reinterpret_cast<const char*>(&msg) + desc.GetOffset());
    const EnumFieldDescriptor* field_desc = dynamic_cast<const EnumFieldDescriptor*>(&desc);
    assert(field_desc != nullptr);
    const EnumDescriptor* enum_desc = field_desc->GetEnumDescriptor();
    assert(enum_desc != nullptr);
    return enum_desc->FindName(value);
}

bool SetEnumName(Message& msg, const FieldDescriptor& desc, std::string_view name)
{
    assert(desc.GetCppType() == CPPTYPE_ENUM);
    const EnumFieldDescriptor* field_desc = dynamic_cast<const EnumFieldDescriptor*>(&desc);
    assert(field_desc != nullptr);
    const EnumDescriptor* enum_desc = field_desc->GetEnumDescriptor();
    assert(enum_desc != nullptr);

    int32_t value = 0;
    if (!enum_desc->ParseName(name, value))
    {
        return false;
    }
    *reinterpret_cast<int32_t*>(reinterpret_cast<char*>(&msg) + desc.GetOffset()) = value;
    return true;
}

const Message& GetMessage(const Message& msg, const FieldDescriptor& desc)
{
    assert(desc.GetCppType() == CPPTYPE_MESSAGE);
    return *reinterpret_cast<const Message*>(reinterpret_cast<const char*>(&msg) + desc.GetOffset());
}

Message& GetMessage(Message& msg, const FieldDescriptor& desc)
{
    assert(desc.GetCppType() == CPPTYPE_MESSAGE);
    return *reinterpret_cast<Message*>(reinterpret_cast<char*>(&msg) + desc.GetOffset());
}

RepeatedFieldDescriptor::Iterator* RepeatedNewIterator(const Message& msg, const FieldDescriptor& desc)
{
    assert(desc.GetCppType() == CPPTYPE_VECTOR || desc.GetCppType() == CPPTYPE_LIST);
    const RepeatedFieldDescriptor* field_desc = dynamic_cast<const RepeatedFieldDescriptor*>(&desc);
    assert(field_desc != nullptr);
    return field_desc->NewIterator(msg);
}

int32_t RepeatedGetEnum(const FieldDescriptor& desc, RepeatedFieldDescriptor::Iterator& it)
{
    assert(desc.GetCppType() == CPPTYPE_VECTOR || desc.GetCppType() == CPPTYPE_LIST);
    const RepeatedFieldDescriptor* field_desc = dynamic_cast<const RepeatedFieldDescriptor*>(&desc);
    (void)field_desc;
    assert(field_desc != nullptr);
    assert(field_desc->GetValueCppType() == CPPTYPE_ENUM);
    return it.Get<int32_t>();
}

bool RepeatedAddEnum(Message& msg, const FieldDescriptor& desc, int32_t value)
{
    assert(desc.GetCppType() == CPPTYPE_VECTOR || desc.GetCppType() == CPPTYPE_LIST);
    const RepeatedFieldDescriptor* field_desc = dynamic_cast<const RepeatedFieldDescriptor*>(&desc);
    assert(field_desc != nullptr);
    assert(field_desc->GetValueCppType() == CPPTYPE_ENUM);
    const EnumDescriptor* enum_desc = field_desc->GetEnumDescriptor();
    assert(enum_desc != nullptr);

    if (!enum_desc->IsValid(value))
    {
        return false;
    }
    field_desc->Add<int32_t>(msg) = value;
    return true;
}

std::string_view RepeatedGetEnumName(const FieldDescriptor& desc, RepeatedFieldDescriptor::Iterator& it)
{
    assert(desc.GetCppType() == CPPTYPE_VECTOR || desc.GetCppType() == CPPTYPE_LIST);
    const RepeatedFieldDescriptor* field_desc = dynamic_cast<const RepeatedFieldDescriptor*>(&desc);
    assert(field_desc != nullptr);
    assert(field_desc->GetValueCppType() == CPPTYPE_ENUM);
    const EnumDescriptor* enum_desc = field_desc->GetEnumDescriptor();
    assert(enum_desc != nullptr);
    return enum_desc->FindName(it.Get<int32_t>());
}

bool RepeatedAddEnumName(Message& msg, const FieldDescriptor& desc, std::string_view name)
{
    assert(desc.GetCppType() == CPPTYPE_VECTOR || desc.GetCppType() == CPPTYPE_LIST);
    const RepeatedFieldDescriptor* field_desc = dynamic_cast<const RepeatedFieldDescriptor*>(&desc);
    assert(field_desc != nullptr);
    assert(field_desc->GetValueCppType() == CPPTYPE_ENUM);
    const EnumDescriptor* enum_desc = field_desc->GetEnumDescriptor();
    assert(enum_desc != nullptr);

    int32_t value = 0;
    if (!enum_desc->ParseName(name, value))
    {
        return false;
    }
    field_desc->Add<int32_t>(msg) = value;
    return true;
}

const Message& RepeatedGetMessage(const FieldDescriptor& desc, RepeatedFieldDescriptor::Iterator& it)
{
    assert(desc.GetCppType() == CPPTYPE_VECTOR || desc.GetCppType() == CPPTYPE_LIST);
    const RepeatedFieldDescriptor* field_desc = dynamic_cast<const RepeatedFieldDescriptor*>(&desc);
    (void)field_desc;
    assert(field_desc != nullptr);
    assert(field_desc->GetValueCppType() == CPPTYPE_MESSAGE);
    return it.Get<Message>();
}

Message& RepeatedAddMessage(Message& msg, const FieldDescriptor& desc)
{
    assert(desc.GetCppType() == CPPTYPE_VECTOR || desc.GetCppType() == CPPTYPE_LIST);
    const RepeatedFieldDescriptor* field_desc = dynamic_cast<const RepeatedFieldDescriptor*>(&desc);
    assert(field_desc != nullptr);
    assert(field_desc->GetValueCppType() == CPPTYPE_MESSAGE);
    return field_desc->Add<Message>(msg);
}

MapFieldDescriptor::Iterator* MapNewIterator(const Message& msg, const FieldDescriptor& desc)
{
    assert(desc.GetCppType() == CPPTYPE_MAP || desc.GetCppType() == CPPTYPE_UNORDERED_MAP);
    const MapFieldDescriptor* field_desc = dynamic_cast<const MapFieldDescriptor*>(&desc);
    assert(field_desc != nullptr);
    return field_desc->NewIterator(msg);
}

int32_t MapGetEnumValue(const FieldDescriptor& desc, MapFieldDescriptor::Iterator& it)
{
    assert(desc.GetCppType() == CPPTYPE_MAP || desc.GetCppType() == CPPTYPE_UNORDERED_MAP);
    const MapFieldDescriptor* field_desc = dynamic_cast<const MapFieldDescriptor*>(&desc);
    (void)field_desc;
    assert(field_desc != nullptr);
    assert(field_desc->GetValueCppType() == CPPTYPE_ENUM);
    return it.GetValue<int32_t>();
}

std::string_view MapGetEnumValueName(const FieldDescriptor& desc, MapFieldDescriptor::Iterator& it)
{
    assert(desc.GetCppType() == CPPTYPE_MAP || desc.GetCppType() == CPPTYPE_UNORDERED_MAP);
    const MapFieldDescriptor* field_desc = dynamic_cast<const MapFieldDescriptor*>(&desc);
    assert(field_desc != nullptr);
    assert(field_desc->GetValueCppType() == CPPTYPE_ENUM);
    const EnumDescriptor* enum_desc = field_desc->GetEnumDescriptor();
    assert(enum_desc != nullptr);
    return enum_desc->FindName(it.GetValue<int32_t>());
}

const Message& MapGetMessageValue(const FieldDescriptor& desc, MapFieldDescriptor::Iterator& it)
{
    assert(desc.GetCppType() == CPPTYPE_MAP || desc.GetCppType() == CPPTYPE_UNORDERED_MAP);
    const MapFieldDescriptor* field_desc = dynamic_cast<const MapFieldDescriptor*>(&desc);
    (void)field_desc;
    assert(field_desc != nullptr);
    assert(field_desc->GetValueCppType() == CPPTYPE_MESSAGE);
    return it.GetValue<Message>();
}

}
