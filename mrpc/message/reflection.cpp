#include <mrpc/message/reflection.h>

int32_t mrpc::reflection::GetEnumValue(const Message& msg, const FieldDescriptor& desc)
{
    assert(desc.GetCppType() == CPPTYPE_ENUM);
    return *reinterpret_cast<const int32_t*>(reinterpret_cast<const char*>(&msg) + desc.GetOffset());
}
