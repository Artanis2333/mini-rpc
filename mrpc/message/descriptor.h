#pragma once

#include <cstdint>
#include <initializer_list>
#include <string_view>
#include <vector>

#include <mrpc/message/message.h>
#include <mrpc/options.mrpc.h>

namespace mrpc
{

class FieldDescriptor;

class EnumDescriptor final
{
public:
    EnumDescriptor(std::string_view name,
            std::string_view full_name,
            bool(*value_checker)(int32_t),
            std::string_view (*name_finder)(int32_t),
            bool (*name_parser)(std::string_view, int32_t&));
    ~EnumDescriptor() = default;

    inline std::string_view GetName() const { return name_; }
    inline std::string_view GetFullName() const { return full_name_; }

    bool IsValid(int32_t value) const { return value_checker_(value); }
    std::string_view FindName(int32_t value) const { return name_finder_(value); }
    bool ParseName(std::string_view name, int32_t& value) const { return name_parser_(name, value); }

private:
    std::string_view name_;
    std::string_view full_name_;
    bool(*value_checker_)(int32_t);
    std::string_view (*name_finder_)(int32_t);
    bool (*name_parser_)(std::string_view, int32_t&);
};

class Descriptor
{
public:
    Descriptor(std::string_view name,
            std::string_view full_name,
            std::initializer_list<const FieldDescriptor*> fields);
    virtual ~Descriptor() = default;

    inline std::string_view GetName() const { return name_; }
    inline std::string_view GetFullName() const { return full_name_; }
    inline const std::vector<const FieldDescriptor*>& GetFields() const { return fields_; }

    const FieldDescriptor* FindFieldByName(std::string_view name) const;

    virtual Message* New() const = 0;
    virtual Message* Clone(const Message& msg) const = 0;

private:
    std::string_view name_;
    std::string_view full_name_;
    std::vector<const FieldDescriptor*> fields_;
};

class FieldDescriptor
{
public:
    FieldDescriptor(std::string_view name,
            CppType cpp_type,
            size_t offset);
    virtual ~FieldDescriptor() = default;

    inline std::string_view GetName() const { return name_; }
    inline CppType GetCppType() const { return cpp_type_; }
    inline size_t GetOffset() const { return offset_; }

private:
    std::string_view name_;
    CppType cpp_type_ = CPPTYPE_UNKNOWN;
    size_t offset_ = 0;
};

class EnumFieldDescriptor : public FieldDescriptor
{
public:
    EnumFieldDescriptor(std::string_view name,
            CppType cpp_type,
            size_t offset,
            const EnumDescriptor* enum_descriptor);

    inline const EnumDescriptor* GetEnumDescriptor() const { return enum_descriptor_; }

private:
    const EnumDescriptor* enum_descriptor_ = nullptr;
};

class MessageFieldDescriptor : public FieldDescriptor
{
public:
    MessageFieldDescriptor(std::string_view name,
            CppType cpp_type,
            size_t offset,
            const Descriptor* descriptor);

    inline const Descriptor* GetDescriptor() const { return descriptor_; }

private:
    const Descriptor* descriptor_ = nullptr;
};

class RepeatedFieldDescriptor : public FieldDescriptor
{
public:
    class Iterator
    {
    public:
        Iterator() = default;
        virtual ~Iterator() = default;

        virtual bool HasNext() const = 0;
        virtual void Next() = 0;

        template<typename T>
        inline const T& Get() const
        {
            return *reinterpret_cast<const T*>(GetDataPtr());
        }

    protected:
        virtual const void* GetDataPtr() const = 0;
    };

    RepeatedFieldDescriptor(std::string_view name,
            CppType cpp_type,
            size_t offset,
            CppType value_cpp_type);
    RepeatedFieldDescriptor(std::string_view name,
            CppType cpp_type,
            size_t offset,
            CppType value_cpp_type,
            const EnumDescriptor* enum_descriptor);
    RepeatedFieldDescriptor(std::string_view name,
            CppType cpp_type,
            size_t offset,
            CppType value_cpp_type,
            const Descriptor* descriptor);

    inline CppType GetValueCppType() const { return value_cpp_type_; }
    inline const EnumDescriptor* GetEnumDescriptor() const { return enum_descriptor_; }
    inline const Descriptor* GetDescriptor() const { return descriptor_; }

    virtual Iterator* NewIterator(const Message& msg) const = 0;

    template<typename T>
    inline T& Add(Message& msg) const
    {
        return *reinterpret_cast<T*>(AddDataPtr(msg));
    }

protected:
    virtual void* AddDataPtr(Message& msg) const = 0;

private:
    CppType value_cpp_type_ = CPPTYPE_UNKNOWN;
    union
    {
        const EnumDescriptor* enum_descriptor_ = nullptr;
        const Descriptor* descriptor_;
    };
};

class MapFieldDescriptor : public FieldDescriptor
{
public:
    class Iterator
    {
    public:
        Iterator() = default;
        virtual ~Iterator() = default;

        virtual bool HasNext() const = 0;
        virtual void Next() = 0;

        template<typename T>
        inline const T& GetKey() const
        {
            return *reinterpret_cast<const T*>(GetKeyDataPtr());
        }

        template<typename T>
        inline const T& GetValue() const
        {
            return *reinterpret_cast<const T*>(GetValueDataPtr());
        }

    protected:
        virtual const void* GetKeyDataPtr() const = 0;
        virtual const void* GetValueDataPtr() const = 0;
    };

    MapFieldDescriptor(std::string_view name,
            CppType cpp_type,
            size_t offset,
            CppType key_cpp_type,
            CppType value_cpp_type);
    MapFieldDescriptor(std::string_view name,
            CppType cpp_type,
            size_t offset,
            CppType key_cpp_type,
            CppType value_cpp_type,
            const EnumDescriptor* enum_descriptor);
    MapFieldDescriptor(std::string_view name,
            CppType cpp_type,
            size_t offset,
            CppType key_cpp_type,
            CppType value_cpp_type,
            const Descriptor* descriptor);

    inline CppType GetKeyCppType() const { return key_cpp_type_; }
    inline CppType GetValueCppType() const { return value_cpp_type_; }
    inline const EnumDescriptor* GetEnumDescriptor() const { return enum_descriptor_; }
    inline const Descriptor* GetDescriptor() const { return descriptor_; }

    virtual Iterator* NewIterator(const Message& msg) const = 0;

    template<typename K, typename V>
    inline V* Find(Message& msg, const K& key, bool create_if_not_exist) const
    {
        return reinterpret_cast<V*>(FindDataPtr(msg, &key, create_if_not_exist));
    }

protected:
    virtual void* FindDataPtr(Message& msg, const void* key, bool create_if_not_exist) const = 0;

private:
    CppType key_cpp_type_ = CPPTYPE_UNKNOWN;
    CppType value_cpp_type_ = CPPTYPE_UNKNOWN;
    union
    {
        const EnumDescriptor* enum_descriptor_ = nullptr;
        const Descriptor* descriptor_;
    };
};

class DescriptorPool final
{
public:
    static const Descriptor* FindDescriptorByFullName(std::string_view full_name);
};

}
