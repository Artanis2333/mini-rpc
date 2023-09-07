#pragma once

#include <mrpc/message/reflection.h>

namespace mrpc
{

template<typename T, typename A>
constexpr size_t OffsetOfImpl(T const* t, A T::* a)
{
    return (char const*)&(t->*a) - (char const*)t;
}

#define OffsetOf(Type, Attr) OffsetOfImpl((Type const*)nullptr, &Type::Attr)

template<typename T>
class DescriptorImpl : public Descriptor
{
public:
    DescriptorImpl(std::string_view name,
            std::string_view full_name,
            std::initializer_list<const FieldDescriptor*> fields);

    Message* New() const override;
    Message* Clone(const Message& msg) const override;
};

template<typename T>
DescriptorImpl<T>::DescriptorImpl(std::string_view name,
        std::string_view full_name,
        std::initializer_list<const FieldDescriptor*> fields) :
    Descriptor(name, full_name, fields)
{
}

template<typename T>
Message* DescriptorImpl<T>::New() const
{
    return new T;
}

template<typename T>
Message* DescriptorImpl<T>::Clone(const Message& msg) const
{
    const T* t = dynamic_cast<const T*>(&msg);
    if (t == nullptr) return nullptr;

    return new T(*t);
}

};
