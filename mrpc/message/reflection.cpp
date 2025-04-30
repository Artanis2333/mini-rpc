#include <mrpc/message/reflection.h>

namespace mrpc
{

int32_t Reflection::GetEnum(const Message& msg, const FieldDescriptor& desc)
{
    assert(desc.GetCppType() == CPPTYPE_ENUM);
    return *reinterpret_cast<const int32_t*>(reinterpret_cast<const char*>(&msg) + desc.GetOffset());
}

bool Reflection::SetEnum(Message& msg, const FieldDescriptor& desc, int32_t value)
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

std::string_view Reflection::GetEnumName(const Message& msg, const FieldDescriptor& desc)
{
    assert(desc.GetCppType() == CPPTYPE_ENUM);
    int32_t value = *reinterpret_cast<const int32_t*>(reinterpret_cast<const char*>(&msg) + desc.GetOffset());
    const EnumFieldDescriptor* field_desc = dynamic_cast<const EnumFieldDescriptor*>(&desc);
    assert(field_desc != nullptr);
    const EnumDescriptor* enum_desc = field_desc->GetEnumDescriptor();
    assert(enum_desc != nullptr);
    return enum_desc->FindName(value);
}

bool Reflection::SetEnumName(Message& msg, const FieldDescriptor& desc, std::string_view name)
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

const Message& Reflection::GetMessage(const Message& msg, const FieldDescriptor& desc)
{
    assert(desc.GetCppType() == CPPTYPE_MESSAGE);
    return *reinterpret_cast<const Message*>(reinterpret_cast<const char*>(&msg) + desc.GetOffset());
}

Message& Reflection::GetMessage(Message& msg, const FieldDescriptor& desc)
{
    assert(desc.GetCppType() == CPPTYPE_MESSAGE);
    return *reinterpret_cast<Message*>(reinterpret_cast<char*>(&msg) + desc.GetOffset());
}

RepeatedFieldDescriptor::Iterator* Reflection::RepeatedNewIterator(const Message& msg, const FieldDescriptor& desc)
{
    assert(IsRepeatedCppType(desc.GetCppType()));
    const RepeatedFieldDescriptor* field_desc = dynamic_cast<const RepeatedFieldDescriptor*>(&desc);
    assert(field_desc != nullptr);
    return field_desc->NewIterator(msg);
}

int32_t Reflection::RepeatedGetEnum(const FieldDescriptor& desc, RepeatedFieldDescriptor::Iterator& it)
{
    assert(IsRepeatedCppType(desc.GetCppType()));
    const RepeatedFieldDescriptor* field_desc = dynamic_cast<const RepeatedFieldDescriptor*>(&desc);
    (void)field_desc;
    assert(field_desc != nullptr);
    assert(field_desc->GetValueCppType() == CPPTYPE_ENUM);
    return it.Get<int32_t>();
}

bool Reflection::RepeatedAddEnum(Message& msg, const FieldDescriptor& desc, int32_t value)
{
    assert(IsRepeatedCppType(desc.GetCppType()));
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

std::string_view Reflection::RepeatedGetEnumName(const FieldDescriptor& desc, RepeatedFieldDescriptor::Iterator& it)
{
    assert(IsRepeatedCppType(desc.GetCppType()));
    const RepeatedFieldDescriptor* field_desc = dynamic_cast<const RepeatedFieldDescriptor*>(&desc);
    assert(field_desc != nullptr);
    assert(field_desc->GetValueCppType() == CPPTYPE_ENUM);
    const EnumDescriptor* enum_desc = field_desc->GetEnumDescriptor();
    assert(enum_desc != nullptr);
    return enum_desc->FindName(it.Get<int32_t>());
}

bool Reflection::RepeatedAddEnumName(Message& msg, const FieldDescriptor& desc, std::string_view name)
{
    assert(IsRepeatedCppType(desc.GetCppType()));
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

const Message& Reflection::RepeatedGetMessage(const FieldDescriptor& desc, RepeatedFieldDescriptor::Iterator& it)
{
    assert(IsRepeatedCppType(desc.GetCppType()));
    const RepeatedFieldDescriptor* field_desc = dynamic_cast<const RepeatedFieldDescriptor*>(&desc);
    (void)field_desc;
    assert(field_desc != nullptr);
    assert(field_desc->GetValueCppType() == CPPTYPE_MESSAGE);
    return it.Get<Message>();
}

Message& Reflection::RepeatedAddMessage(Message& msg, const FieldDescriptor& desc)
{
    assert(IsRepeatedCppType(desc.GetCppType()));
    const RepeatedFieldDescriptor* field_desc = dynamic_cast<const RepeatedFieldDescriptor*>(&desc);
    assert(field_desc != nullptr);
    assert(field_desc->GetValueCppType() == CPPTYPE_MESSAGE);
    return field_desc->Add<Message>(msg);
}

MapFieldDescriptor::Iterator* Reflection::MapNewIterator(const Message& msg, const FieldDescriptor& desc)
{
    assert(IsMapCppType(desc.GetCppType()));
    const MapFieldDescriptor* field_desc = dynamic_cast<const MapFieldDescriptor*>(&desc);
    assert(field_desc != nullptr);
    return field_desc->NewIterator(msg);
}

int32_t Reflection::MapGetEnumValue(const FieldDescriptor& desc, MapFieldDescriptor::Iterator& it)
{
    assert(IsMapCppType(desc.GetCppType()));
    const MapFieldDescriptor* field_desc = dynamic_cast<const MapFieldDescriptor*>(&desc);
    (void)field_desc;
    assert(field_desc != nullptr);
    assert(field_desc->GetValueCppType() == CPPTYPE_ENUM);
    return it.GetValue<int32_t>();
}

std::string_view Reflection::MapGetEnumValueName(const FieldDescriptor& desc, MapFieldDescriptor::Iterator& it)
{
    assert(IsMapCppType(desc.GetCppType()));
    const MapFieldDescriptor* field_desc = dynamic_cast<const MapFieldDescriptor*>(&desc);
    assert(field_desc != nullptr);
    assert(field_desc->GetValueCppType() == CPPTYPE_ENUM);
    const EnumDescriptor* enum_desc = field_desc->GetEnumDescriptor();
    assert(enum_desc != nullptr);
    return enum_desc->FindName(it.GetValue<int32_t>());
}

const Message& Reflection::MapGetMessageValue(const FieldDescriptor& desc, MapFieldDescriptor::Iterator& it)
{
    assert(IsMapCppType(desc.GetCppType()));
    const MapFieldDescriptor* field_desc = dynamic_cast<const MapFieldDescriptor*>(&desc);
    (void)field_desc;
    assert(field_desc != nullptr);
    assert(field_desc->GetValueCppType() == CPPTYPE_MESSAGE);
    return it.GetValue<Message>();
}

}
