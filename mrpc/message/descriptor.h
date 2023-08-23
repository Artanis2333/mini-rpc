#pragma once

#include <stdint.h>
#include <initializer_list>
#include <string_view>
#include <vector>
#include <unordered_map>

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
    RepeatedFieldDescriptor(std::string_view name,
            CppType cpp_type,
            size_t offset,
            CppType value_cpp_type);
    RepeatedFieldDescriptor(std::string_view name,
            CppType cpp_type,
            size_t offset,
            CppType value_cpp_type,
            const Descriptor* value_descriptor);

    inline CppType GetValueCppType() const { return value_cpp_type_; }
    inline const Descriptor* GetValueDescriptor() const { return value_descriptor_; }

private:
    CppType value_cpp_type_ = CPPTYPE_UNKNOWN;
    const Descriptor* value_descriptor_ = nullptr;
};

class DescriptorPool
{
public:
    const Descriptor* FindDescriptorByFullName(std::string_view full_name) const;

    static DescriptorPool* GetInstance();

private:
    std::unordered_map<std::string_view, const Descriptor*> pool_;
    static DescriptorPool* instance_;

    void AddDescriptorByFullName(std::string_view full_name, const Descriptor* descriptor);

    friend class Descriptor;
};

};
