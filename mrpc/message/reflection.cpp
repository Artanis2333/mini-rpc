#include <mrpc/message/reflection.h>

mrpc::FieldDescriptor::FieldDescriptor(std::string_view name,
        CppType cpp_type,
        size_t offset) :
    name_(name),
    cpp_type_(cpp_type),
    offset_(offset)
{
}

mrpc::FieldDescriptor::FieldDescriptor(std::string_view name,
        CppType cpp_type,
        size_t offset,
        const Descriptor* message_type) :
    name_(name),
    cpp_type_(cpp_type),
    offset_(offset),
    message_type_(message_type)
{
}

int32_t mrpc::reflection::GetInt32(const Message& msg, const FieldDescriptor& desc)
{
    return *reinterpret_cast<const int32_t*>(reinterpret_cast<const char*>(&msg) + desc.GetOffset());
}

void mrpc::reflection::SetInt32(Message& msg, const FieldDescriptor& desc, int32_t value)
{
    *reinterpret_cast<int32_t*>(reinterpret_cast<char*>(&msg) + desc.GetOffset()) = value;
}
