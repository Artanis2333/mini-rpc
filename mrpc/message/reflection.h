#pragma once

#include <assert.h>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <mrpc/message/descriptor.h>

namespace mrpc::reflection
{

template<typename T>
struct ReflectionCppTypeTraits
{
    static const CppType cpp_type = CPPTYPE_UNKNOWN;
};

#define REFLECTION_CPP_TYPE_TRAITS_HELPER(enum_type, type)  \
template<>                                                  \
struct ReflectionCppTypeTraits<type>                        \
{                                                           \
    static const CppType cpp_type = enum_type;              \
};

REFLECTION_CPP_TYPE_TRAITS_HELPER(CPPTYPE_INT32, int32_t)
REFLECTION_CPP_TYPE_TRAITS_HELPER(CPPTYPE_UINT32, uint32_t)
REFLECTION_CPP_TYPE_TRAITS_HELPER(CPPTYPE_INT64, int64_t)
REFLECTION_CPP_TYPE_TRAITS_HELPER(CPPTYPE_UINT64, uint64_t)
REFLECTION_CPP_TYPE_TRAITS_HELPER(CPPTYPE_FLOAT, float)
REFLECTION_CPP_TYPE_TRAITS_HELPER(CPPTYPE_DOUBLE, double)
REFLECTION_CPP_TYPE_TRAITS_HELPER(CPPTYPE_BOOL, bool)
//REFLECTION_CPP_TYPE_TRAITS_HELPER(CPPTYPE_ENUM, int32_t)
REFLECTION_CPP_TYPE_TRAITS_HELPER(CPPTYPE_STRING, std::string)
//REFLECTION_CPP_TYPE_TRAITS_HELPER(CPPTYPE_MESSAGE, Message)

template<typename T>
const T* GetPtr(const Message& msg, const FieldDescriptor& desc)
{
    assert(desc.GetCppType() == ReflectionCppTypeTraits<T>::cpp_type);
    return reinterpret_cast<const T*>(reinterpret_cast<const char*>(&msg) + desc.GetOffset());
}

template<typename T>
T* GetPtr(Message& msg, const FieldDescriptor& desc)
{
    assert(desc.GetCppType() == ReflectionCppTypeTraits<T>::cpp_type);
    return reinterpret_cast<T*>(reinterpret_cast<char*>(&msg) + desc.GetOffset());
}

int32_t GetEnumValue(const Message& msg, const FieldDescriptor& desc);

template<typename T>
const std::vector<T>* GetVectorPtr(const Message& msg, const FieldDescriptor& desc)
{
    assert(desc.GetCppType() == CPPTYPE_VECTOR);
    const RepeatedFieldDescriptor* field_desc = dynamic_cast<const RepeatedFieldDescriptor*>(&desc);
    assert(field_desc != nullptr);
    assert(field_desc->GetValueCppType() == ReflectionCppTypeTraits<T>::cpp_type);
    return reinterpret_cast<const std::vector<T>*>(reinterpret_cast<const char*>(&msg) + desc.GetOffset());
}

template<typename T>
std::vector<T>* GetVectorPtr(Message& msg, const FieldDescriptor& desc)
{
    assert(desc.GetCppType() == CPPTYPE_VECTOR);
    const RepeatedFieldDescriptor* field_desc = dynamic_cast<const RepeatedFieldDescriptor*>(&desc);
    assert(field_desc != nullptr);
    assert(field_desc->GetValueCppType() == ReflectionCppTypeTraits<T>::cpp_type);
    return reinterpret_cast<std::vector<T>*>(reinterpret_cast<char*>(&msg) + desc.GetOffset());
}

template<typename T>
const std::list<T>* GetListPtr(const Message& msg, const FieldDescriptor& desc)
{
    assert(desc.GetCppType() == CPPTYPE_LIST);
    const RepeatedFieldDescriptor* field_desc = dynamic_cast<const RepeatedFieldDescriptor*>(&desc);
    assert(field_desc != nullptr);
    assert(field_desc->GetValueCppType() == ReflectionCppTypeTraits<T>::cpp_type);
    return reinterpret_cast<const std::list<T>*>(reinterpret_cast<const char*>(&msg) + desc.GetOffset());
}

template<typename T>
std::list<T>* GetListPtr(Message& msg, const FieldDescriptor& desc)
{
    assert(desc.GetCppType() == CPPTYPE_LIST);
    const RepeatedFieldDescriptor* field_desc = dynamic_cast<const RepeatedFieldDescriptor*>(&desc);
    assert(field_desc != nullptr);
    assert(field_desc->GetValueCppType() == ReflectionCppTypeTraits<T>::cpp_type);
    return reinterpret_cast<std::list<T>*>(reinterpret_cast<char*>(&msg) + desc.GetOffset());
}

};
