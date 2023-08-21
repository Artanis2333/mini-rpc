#pragma once

#include <stdint.h>
#include <string_view>
#include <vector>

#include <mrpc/message/message.h>
#include <mrpc/options.mrpc.h>

namespace mrpc
{

class Descriptor;

class FieldDescriptor
{
public:
    FieldDescriptor(std::string_view name,
            CppType cpp_type,
            size_t offset);
    FieldDescriptor(std::string_view name,
            CppType cpp_type,
            size_t offset,
            const Descriptor* message_type);
    ~FieldDescriptor() = default;

    inline std::string_view GetName() const { return name_; }
    inline CppType GetCppType() const { return cpp_type_; }
    inline size_t GetOffset() const { return offset_; }
    inline const Descriptor* GetMessageType() const { return message_type_; }

private:
    std::string_view name_;
    CppType cpp_type_ = CPPTYPE_UNKNOWN;
    size_t offset_ = 0;
    const Descriptor* message_type_ = nullptr;
};

class Descriptor
{
public:
    inline std::string_view GetName() const { return name_; }
    inline void SetName(std::string_view name) { name_ = name; }
    inline std::string_view GetFullName() const { return full_name_; }
    inline void SetFullName(std::string_view full_name) { full_name_ = full_name; }

    template <typename... Args>
    inline void EmplaceField(Args&&... args) { fields_.emplace_back(args...); }

private:
    std::string_view name_;
    std::string_view full_name_;
    std::vector<FieldDescriptor> fields_;
};

namespace reflection
{

int32_t GetInt32(const Message& msg, const FieldDescriptor& desc);
void SetInt32(Message& msg, const FieldDescriptor& desc, int32_t value);

};

};
