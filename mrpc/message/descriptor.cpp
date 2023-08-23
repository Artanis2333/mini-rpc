#include <assert.h>
#include <mrpc/message/descriptor.h>

mrpc::EnumDescriptor::EnumDescriptor(std::string_view name,
        std::string_view full_name,
        bool(*value_checker)(int32_t),
        std::string_view (*name_finder)(int32_t),
        bool (*name_parser)(std::string_view, int32_t&)) :
    name_(name),
    full_name_(full_name),
    value_checker_(value_checker),
    name_finder_(name_finder),
    name_parser_(name_parser)
{
}

mrpc::Descriptor::Descriptor(std::string_view name,
        std::string_view full_name,
        std::initializer_list<const FieldDescriptor*> fields) :
    name_(name),
    full_name_(full_name),
    fields_(fields)
{
    DescriptorPool::GetInstance()->AddDescriptorByFullName(full_name_, this);
}

const mrpc::FieldDescriptor* mrpc::Descriptor::FindFieldByName(std::string_view name) const
{
    for (auto& field : fields_)
    {
        if (field->GetName() == name)
        {
            return field;
        }
    }
    return nullptr;
}

mrpc::FieldDescriptor::FieldDescriptor(std::string_view name,
        CppType cpp_type,
        size_t offset) :
    name_(name),
    cpp_type_(cpp_type),
    offset_(offset)
{
}

mrpc::EnumFieldDescriptor::EnumFieldDescriptor(std::string_view name,
        CppType cpp_type,
        size_t offset,
        const EnumDescriptor* enum_descriptor) :
    FieldDescriptor(name, cpp_type, offset),
    enum_descriptor_(enum_descriptor)
{
}

mrpc::MessageFieldDescriptor::MessageFieldDescriptor(std::string_view name,
        CppType cpp_type,
        size_t offset,
        const Descriptor* descriptor) :
    FieldDescriptor(name, cpp_type, offset),
    descriptor_(descriptor)
{
}

mrpc::DescriptorPool* mrpc::DescriptorPool::instance_ = nullptr;

const mrpc::Descriptor* mrpc::DescriptorPool::FindDescriptorByFullName(std::string_view full_name) const
{
    auto it = pool_.find(full_name);
    if (it == pool_.end()) return nullptr;
    return it->second;
}

void mrpc::DescriptorPool::AddDescriptorByFullName(std::string_view full_name, const mrpc::Descriptor* descriptor)
{
    assert(pool_.find(full_name) == pool_.end());
    pool_.emplace(full_name, descriptor);
}

mrpc::DescriptorPool* mrpc::DescriptorPool::GetInstance()
{
    if (instance_ == nullptr)
    {
        instance_ = new DescriptorPool;
    }
    return instance_;
}
