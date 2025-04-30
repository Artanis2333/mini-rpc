#pragma once

#include <cassert>
#include <memory>
#include <mrpc/message/descriptor.h>

namespace mrpc
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

class Reflection
{
public:

    static inline constexpr bool IsRepeatedCppType(CppType cpp_type)
    {
        return cpp_type == CPPTYPE_VECTOR || cpp_type == CPPTYPE_LIST;
    }

    static inline constexpr bool IsMapCppType(CppType cpp_type)
    {
        return cpp_type == CPPTYPE_MAP || cpp_type == CPPTYPE_UNORDERED_MAP;
    }

    template<typename T>
    static inline const T& Get(const Message& msg, const FieldDescriptor& desc)
    {
        assert(desc.GetCppType() == ReflectionCppTypeTraits<T>::cpp_type);
        return *reinterpret_cast<const T*>(reinterpret_cast<const char*>(&msg) + desc.GetOffset());
    }

    template<typename T>
    static inline void Set(Message& msg, const FieldDescriptor& desc, const T& t)
    {
        assert(desc.GetCppType() == ReflectionCppTypeTraits<T>::cpp_type);
        *reinterpret_cast<T*>(reinterpret_cast<char*>(&msg) + desc.GetOffset()) = t;
    }

    static int32_t GetEnum(const Message& msg, const FieldDescriptor& desc);
    static bool SetEnum(Message& msg, const FieldDescriptor& desc, int32_t value);

    static std::string_view GetEnumName(const Message& msg, const FieldDescriptor& desc);
    static bool SetEnumName(Message& msg, const FieldDescriptor& desc, std::string_view name);

    static const Message& GetMessage(const Message& msg, const FieldDescriptor& desc);
    static Message& GetMessage(Message& msg, const FieldDescriptor& desc);

    static RepeatedFieldDescriptor::Iterator* RepeatedNewIterator(const Message& msg, const FieldDescriptor& desc);

    template<typename T>
    static inline const T& RepeatedGet(const FieldDescriptor& desc, RepeatedFieldDescriptor::Iterator& it)
    {
        assert(IsRepeatedCppType(desc.GetCppType()));
        const RepeatedFieldDescriptor* field_desc = dynamic_cast<const RepeatedFieldDescriptor*>(&desc);
        (void)field_desc;
        assert(field_desc != nullptr);
        assert(field_desc->GetValueCppType() == ReflectionCppTypeTraits<T>::cpp_type);
        return it.Get<T>();
    }

    template<typename T>
    static inline void RepeatedAdd(Message& msg, const FieldDescriptor& desc, const T& t)
    {
        assert(IsRepeatedCppType(desc.GetCppType()));
        const RepeatedFieldDescriptor* field_desc = dynamic_cast<const RepeatedFieldDescriptor*>(&desc);
        assert(field_desc != nullptr);
        assert(field_desc->GetValueCppType() == ReflectionCppTypeTraits<T>::cpp_type);
        field_desc->Add<T>(msg) = t;
    }

    static int32_t RepeatedGetEnum(const FieldDescriptor& desc, RepeatedFieldDescriptor::Iterator& it);
    static bool RepeatedAddEnum(Message& msg, const FieldDescriptor& desc, int32_t value);

    static std::string_view RepeatedGetEnumName(const FieldDescriptor& desc, RepeatedFieldDescriptor::Iterator& it);
    static bool RepeatedAddEnumName(Message& msg, const FieldDescriptor& desc, std::string_view name);

    static const Message& RepeatedGetMessage(const FieldDescriptor& desc, RepeatedFieldDescriptor::Iterator& it);
    static Message& RepeatedAddMessage(Message& msg, const FieldDescriptor& desc);

    static MapFieldDescriptor::Iterator* MapNewIterator(const Message& msg, const FieldDescriptor& desc);

    template<typename T>
    static inline const T& MapGetKey(const FieldDescriptor& desc, MapFieldDescriptor::Iterator& it)
    {
        assert(IsMapCppType(desc.GetCppType()));
        const MapFieldDescriptor* field_desc = dynamic_cast<const MapFieldDescriptor*>(&desc);
        (void)field_desc;
        assert(field_desc != nullptr);
        assert(field_desc->GetKeyCppType() == ReflectionCppTypeTraits<T>::cpp_type);
        return it.GetKey<T>();
    }

    template<typename T>
    static inline const T& MapGetValue(const FieldDescriptor& desc, MapFieldDescriptor::Iterator& it)
    {
        assert(IsMapCppType(desc.GetCppType()));
        const MapFieldDescriptor* field_desc = dynamic_cast<const MapFieldDescriptor*>(&desc);
        (void)field_desc;
        assert(field_desc != nullptr);
        assert(field_desc->GetValueCppType() == ReflectionCppTypeTraits<T>::cpp_type);
        return it.GetValue<T>();
    }

    static int32_t MapGetEnumValue(const FieldDescriptor& desc, MapFieldDescriptor::Iterator& it);
    static std::string_view MapGetEnumValueName(const FieldDescriptor& desc, MapFieldDescriptor::Iterator& it);
    static const Message& MapGetMessageValue(const FieldDescriptor& desc, MapFieldDescriptor::Iterator& it);

    template<typename K, typename V>
    static inline void MapSet(Message& msg, const FieldDescriptor& desc, const K& key, const V& value)
    {
        assert(IsMapCppType(desc.GetCppType()));
        const MapFieldDescriptor* field_desc = dynamic_cast<const MapFieldDescriptor*>(&desc);
        assert(field_desc != nullptr);
        assert(field_desc->GetKeyCppType() == ReflectionCppTypeTraits<K>::cpp_type);
        assert(field_desc->GetValueCppType() == ReflectionCppTypeTraits<V>::cpp_type);
        *field_desc->Find<K, V>(msg, key, true) = value;
    }

    template<typename K>
    static inline bool MapSetWithEnumValue(Message& msg, const FieldDescriptor& desc, const K& key, int32_t value)
    {
        assert(IsMapCppType(desc.GetCppType()));
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
    static inline bool MapSetWithEnumValueName(Message& msg, const FieldDescriptor& desc, const K& key, std::string_view name)
    {
        assert(IsMapCppType(desc.GetCppType()));
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
    static inline Message& MapSetWithMessageValue(Message& msg, const FieldDescriptor& desc, const K& key)
    {
        assert(IsMapCppType(desc.GetCppType()));
        const MapFieldDescriptor* field_desc = dynamic_cast<const MapFieldDescriptor*>(&desc);
        assert(field_desc != nullptr);
        assert(field_desc->GetKeyCppType() == ReflectionCppTypeTraits<K>::cpp_type);
        assert(field_desc->GetValueCppType() == CPPTYPE_MESSAGE);
        return *field_desc->Find<K, Message>(msg, key, true);
    }

    using RepeatedIteratorPtr = std::unique_ptr<RepeatedFieldDescriptor::Iterator>;
    using MapIteratorPtr = std::unique_ptr<MapFieldDescriptor::Iterator>;
};

}
