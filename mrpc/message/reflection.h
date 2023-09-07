#pragma once

#include <assert.h>
#include <memory>
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
inline const T& Get(const Message& msg, const FieldDescriptor& desc)
{
    assert(desc.GetCppType() == ReflectionCppTypeTraits<T>::cpp_type);
    return *reinterpret_cast<const T*>(reinterpret_cast<const char*>(&msg) + desc.GetOffset());
}

template<typename T>
inline void Set(Message& msg, const FieldDescriptor& desc, const T& t)
{
    assert(desc.GetCppType() == ReflectionCppTypeTraits<T>::cpp_type);
    *reinterpret_cast<T*>(reinterpret_cast<char*>(&msg) + desc.GetOffset()) = t;
}

int32_t GetEnum(const Message& msg, const FieldDescriptor& desc);
bool SetEnum(Message& msg, const FieldDescriptor& desc, int32_t value);

std::string_view GetEnumName(const Message& msg, const FieldDescriptor& desc);
bool SetEnumName(Message& msg, const FieldDescriptor& desc, std::string_view name);

const Message& GetMessage(const Message& msg, const FieldDescriptor& desc);
Message& GetMessage(Message& msg, const FieldDescriptor& desc);

RepeatedFieldDescriptor::Iterator* RepeatedNewIterator(const Message& msg, const FieldDescriptor& desc);

template<typename T>
inline const T& RepeatedGet(const FieldDescriptor& desc, RepeatedFieldDescriptor::Iterator& it)
{
    assert(desc.GetCppType() == CPPTYPE_VECTOR || desc.GetCppType() == CPPTYPE_LIST);
    const RepeatedFieldDescriptor* field_desc = dynamic_cast<const RepeatedFieldDescriptor*>(&desc);
    (void)field_desc;
    assert(field_desc != nullptr);
    assert(field_desc->GetValueCppType() == ReflectionCppTypeTraits<T>::cpp_type);
    return it.Get<T>();
}

template<typename T>
inline void RepeatedAdd(Message& msg, const FieldDescriptor& desc, const T& t)
{
    assert(desc.GetCppType() == CPPTYPE_VECTOR || desc.GetCppType() == CPPTYPE_LIST);
    const RepeatedFieldDescriptor* field_desc = dynamic_cast<const RepeatedFieldDescriptor*>(&desc);
    assert(field_desc != nullptr);
    assert(field_desc->GetValueCppType() == ReflectionCppTypeTraits<T>::cpp_type);
    field_desc->Add<T>(msg) = t;
}

int32_t RepeatedGetEnum(const FieldDescriptor& desc, RepeatedFieldDescriptor::Iterator& it);
bool RepeatedAddEnum(Message& msg, const FieldDescriptor& desc, int32_t value);

std::string_view RepeatedGetEnumName(const FieldDescriptor& desc, RepeatedFieldDescriptor::Iterator& it);
bool RepeatedAddEnumName(Message& msg, const FieldDescriptor& desc, std::string_view name);

const Message& RepeatedGetMessage(const FieldDescriptor& desc, RepeatedFieldDescriptor::Iterator& it);
Message& RepeatedAddMessage(Message& msg, const FieldDescriptor& desc);

MapFieldDescriptor::Iterator* MapNewIterator(const Message& msg, const FieldDescriptor& desc);

template<typename T>
inline const T& MapGetKey(const FieldDescriptor& desc, MapFieldDescriptor::Iterator& it)
{
    assert(desc.GetCppType() == CPPTYPE_MAP || desc.GetCppType() == CPPTYPE_UNORDERED_MAP);
    const MapFieldDescriptor* field_desc = dynamic_cast<const MapFieldDescriptor*>(&desc);
    (void)field_desc;
    assert(field_desc != nullptr);
    assert(field_desc->GetKeyCppType() == ReflectionCppTypeTraits<T>::cpp_type);
    return it.GetKey<T>();
}

template<typename T>
inline const T& MapGetValue(const FieldDescriptor& desc, MapFieldDescriptor::Iterator& it)
{
    assert(desc.GetCppType() == CPPTYPE_MAP || desc.GetCppType() == CPPTYPE_UNORDERED_MAP);
    const MapFieldDescriptor* field_desc = dynamic_cast<const MapFieldDescriptor*>(&desc);
    (void)field_desc;
    assert(field_desc != nullptr);
    assert(field_desc->GetValueCppType() == ReflectionCppTypeTraits<T>::cpp_type);
    return it.GetValue<T>();
}

int32_t MapGetEnumValue(const FieldDescriptor& desc, MapFieldDescriptor::Iterator& it);
std::string_view MapGetEnumValueName(const FieldDescriptor& desc, MapFieldDescriptor::Iterator& it);
const Message& MapGetMessageValue(const FieldDescriptor& desc, MapFieldDescriptor::Iterator& it);

template<typename K, typename V>
inline void MapSet(Message& msg, const FieldDescriptor& desc, const K& key, const V& value)
{
    assert(desc.GetCppType() == CPPTYPE_MAP || desc.GetCppType() == CPPTYPE_UNORDERED_MAP);
    const MapFieldDescriptor* field_desc = dynamic_cast<const MapFieldDescriptor*>(&desc);
    assert(field_desc != nullptr);
    assert(field_desc->GetKeyCppType() == ReflectionCppTypeTraits<K>::cpp_type);
    assert(field_desc->GetValueCppType() == ReflectionCppTypeTraits<V>::cpp_type);
    *field_desc->Find<K, V>(msg, key, true) = value;
}

template<typename K>
inline bool MapSetWithEnumValue(Message& msg, const FieldDescriptor& desc, const K& key, int32_t value)
{
    assert(desc.GetCppType() == CPPTYPE_MAP || desc.GetCppType() == CPPTYPE_UNORDERED_MAP);
    const MapFieldDescriptor* field_desc = dynamic_cast<const MapFieldDescriptor*>(&desc);
    assert(field_desc != nullptr);
    assert(field_desc->GetKeyCppType() == ReflectionCppTypeTraits<K>::cpp_type);
    assert(field_desc->GetValueCppType() == CPPTYPE_ENUM);
    const EnumDescriptor* enum_desc = field_desc->GetEnumDescriptor();
    assert(enum_desc != nullptr);

    if (!enum_desc->IsValid(value))
    {
        return false;
    }
    *field_desc->Find<K, int32_t>(msg, key, true) = value;
    return true;
}

template<typename K>
inline bool MapSetWithEnumValueName(Message& msg, const FieldDescriptor& desc, const K& key, std::string_view name)
{
    assert(desc.GetCppType() == CPPTYPE_MAP || desc.GetCppType() == CPPTYPE_UNORDERED_MAP);
    const MapFieldDescriptor* field_desc = dynamic_cast<const MapFieldDescriptor*>(&desc);
    assert(field_desc != nullptr);
    assert(field_desc->GetKeyCppType() == ReflectionCppTypeTraits<K>::cpp_type);
    assert(field_desc->GetValueCppType() == CPPTYPE_ENUM);
    const EnumDescriptor* enum_desc = field_desc->GetEnumDescriptor();
    assert(enum_desc != nullptr);

    int32_t value = 0;
    if (!enum_desc->ParseName(name, value))
    {
        return false;
    }
    *field_desc->Find<K, int32_t>(msg, key, true) = value;
    return true;
}

template<typename K>
inline Message& MapSetWithMessageValue(Message& msg, const FieldDescriptor& desc, const K& key)
{
    assert(desc.GetCppType() == CPPTYPE_MAP || desc.GetCppType() == CPPTYPE_UNORDERED_MAP);
    const MapFieldDescriptor* field_desc = dynamic_cast<const MapFieldDescriptor*>(&desc);
    assert(field_desc != nullptr);
    assert(field_desc->GetKeyCppType() == ReflectionCppTypeTraits<K>::cpp_type);
    assert(field_desc->GetValueCppType() == CPPTYPE_MESSAGE);
    return *field_desc->Find<K, Message>(msg, key, true);
}

using RepeatedIteratorPtr = std::unique_ptr<RepeatedFieldDescriptor::Iterator>;
using MapIteratorPtr = std::unique_ptr<MapFieldDescriptor::Iterator>;

};
