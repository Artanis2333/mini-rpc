#pragma once

#include <vector>
#include <list>
#include <map>
#include <unordered_map>

#include <mrpc/message/descriptor.h>

namespace mrpc
{

template<typename T, typename A>
constexpr inline size_t OffsetOfImpl(T const* t, A T::* a)
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

template<typename T>
class VectorFieldDescriptorImpl : public RepeatedFieldDescriptor
{
public:
    class IteratorImpl : public RepeatedFieldDescriptor::Iterator
    {
    public:
        IteratorImpl(const typename std::vector<T>& c);

        bool HasNext() const override;
        void Next() override;

    protected:
        const typename std::vector<T>& c_;
        typename std::vector<T>::const_iterator it_;

        const void* GetDataPtr() const override;
    };

    VectorFieldDescriptorImpl(std::string_view name,
            CppType cpp_type,
            size_t offset,
            CppType value_cpp_type);
    VectorFieldDescriptorImpl(std::string_view name,
            CppType cpp_type,
            size_t offset,
            CppType value_cpp_type,
            const EnumDescriptor* enum_descriptor);
    VectorFieldDescriptorImpl(std::string_view name,
            CppType cpp_type,
            size_t offset,
            CppType value_cpp_type,
            const Descriptor* descriptor);

    Iterator* NewIterator(const Message& msg) const override;

protected:
    void* AddDataPtr(Message& msg) const override;
};

template<typename T>
VectorFieldDescriptorImpl<T>::IteratorImpl::IteratorImpl(const typename std::vector<T>& c) :
    c_(c), it_(c.cbegin())
{
}

template<typename T>
bool VectorFieldDescriptorImpl<T>::IteratorImpl::HasNext() const
{
    return it_ != c_.cend();
}

template<typename T>
void VectorFieldDescriptorImpl<T>::IteratorImpl::Next()
{
    ++it_;
}

template<typename T>
const void* VectorFieldDescriptorImpl<T>::IteratorImpl::GetDataPtr() const
{
    return &*it_;
}

template<typename T>
VectorFieldDescriptorImpl<T>::VectorFieldDescriptorImpl(std::string_view name,
        CppType cpp_type,
        size_t offset,
        CppType value_cpp_type) :
    RepeatedFieldDescriptor(name, cpp_type, offset, value_cpp_type)
{
}

template<typename T>
VectorFieldDescriptorImpl<T>::VectorFieldDescriptorImpl(std::string_view name,
        CppType cpp_type,
        size_t offset,
        CppType value_cpp_type,
        const EnumDescriptor* enum_descriptor) :
    RepeatedFieldDescriptor(name, cpp_type, offset, value_cpp_type, enum_descriptor)
{
}

template<typename T>
VectorFieldDescriptorImpl<T>::VectorFieldDescriptorImpl(std::string_view name,
        CppType cpp_type,
        size_t offset,
        CppType value_cpp_type,
        const Descriptor* descriptor) :
    RepeatedFieldDescriptor(name, cpp_type, offset, value_cpp_type, descriptor)
{
}

template<typename T>
RepeatedFieldDescriptor::Iterator* VectorFieldDescriptorImpl<T>::NewIterator(const Message& msg) const
{
    const std::vector<T>* c = reinterpret_cast<const std::vector<T>*>(reinterpret_cast<const char*>(&msg) + this->GetOffset());
    return new IteratorImpl(*c);
}

template<typename T>
void* VectorFieldDescriptorImpl<T>::AddDataPtr(Message& msg) const
{
    std::vector<T>* c = reinterpret_cast<std::vector<T>*>(reinterpret_cast<char*>(&msg) + this->GetOffset());
    return &(c->emplace_back());
}

template<typename T>
class ListFieldDescriptorImpl : public RepeatedFieldDescriptor
{
public:
    class IteratorImpl : public RepeatedFieldDescriptor::Iterator
    {
    public:
        IteratorImpl(const typename std::list<T>& c);

        bool HasNext() const override;
        void Next() override;

    protected:
        const typename std::list<T>& c_;
        typename std::list<T>::const_iterator it_;

        const void* GetDataPtr() const override;
    };

    ListFieldDescriptorImpl(std::string_view name,
            CppType cpp_type,
            size_t offset,
            CppType value_cpp_type);
    ListFieldDescriptorImpl(std::string_view name,
            CppType cpp_type,
            size_t offset,
            CppType value_cpp_type,
            const EnumDescriptor* enum_descriptor);
    ListFieldDescriptorImpl(std::string_view name,
            CppType cpp_type,
            size_t offset,
            CppType value_cpp_type,
            const Descriptor* descriptor);

    Iterator* NewIterator(const Message& msg) const override;

protected:
    void* AddDataPtr(Message& msg) const override;
};

template<typename T>
ListFieldDescriptorImpl<T>::IteratorImpl::IteratorImpl(const typename std::list<T>& c) :
    c_(c), it_(c.cbegin())
{
}

template<typename T>
bool ListFieldDescriptorImpl<T>::IteratorImpl::HasNext() const
{
    return it_ != c_.cend();
}

template<typename T>
void ListFieldDescriptorImpl<T>::IteratorImpl::Next()
{
    ++it_;
}

template<typename T>
const void* ListFieldDescriptorImpl<T>::IteratorImpl::GetDataPtr() const
{
    return &*it_;
}

template<typename T>
ListFieldDescriptorImpl<T>::ListFieldDescriptorImpl(std::string_view name,
        CppType cpp_type,
        size_t offset,
        CppType value_cpp_type) :
    RepeatedFieldDescriptor(name, cpp_type, offset, value_cpp_type)
{
}

template<typename T>
ListFieldDescriptorImpl<T>::ListFieldDescriptorImpl(std::string_view name,
        CppType cpp_type,
        size_t offset,
        CppType value_cpp_type,
        const EnumDescriptor* enum_descriptor) :
    RepeatedFieldDescriptor(name, cpp_type, offset, value_cpp_type, enum_descriptor)
{
}

template<typename T>
ListFieldDescriptorImpl<T>::ListFieldDescriptorImpl(std::string_view name,
        CppType cpp_type,
        size_t offset,
        CppType value_cpp_type,
        const Descriptor* descriptor) :
    RepeatedFieldDescriptor(name, cpp_type, offset, value_cpp_type, descriptor)
{
}

template<typename T>
RepeatedFieldDescriptor::Iterator* ListFieldDescriptorImpl<T>::NewIterator(const Message& msg) const
{
    const std::list<T>* c = reinterpret_cast<const std::list<T>*>(reinterpret_cast<const char*>(&msg) + this->GetOffset());
    return new IteratorImpl(*c);
}

template<typename T>
void* ListFieldDescriptorImpl<T>::AddDataPtr(Message& msg) const
{
    std::list<T>* c = reinterpret_cast<std::list<T>*>(reinterpret_cast<char*>(&msg) + this->GetOffset());
    return &(c->emplace_back());
}

template<typename K, typename V>
class MapFieldDescriptorImpl : public MapFieldDescriptor
{
public:
    class IteratorImpl : public MapFieldDescriptor::Iterator
    {
    public:
        IteratorImpl(const typename std::map<K, V>& c);

        bool HasNext() const override;
        void Next() override;

    protected:
        const typename std::map<K, V>& c_;
        typename std::map<K, V>::const_iterator it_;

        const void* GetKeyDataPtr() const override;
        const void* GetValueDataPtr() const override;
    };

    MapFieldDescriptorImpl(std::string_view name,
            CppType cpp_type,
            size_t offset,
            CppType key_cpp_type,
            CppType value_cpp_type);
    MapFieldDescriptorImpl(std::string_view name,
            CppType cpp_type,
            size_t offset,
            CppType key_cpp_type,
            CppType value_cpp_type,
            const EnumDescriptor* enum_descriptor);
    MapFieldDescriptorImpl(std::string_view name,
            CppType cpp_type,
            size_t offset,
            CppType key_cpp_type,
            CppType value_cpp_type,
            const Descriptor* descriptor);

    Iterator* NewIterator(const Message& msg) const override;

protected:
    void* FindDataPtr(Message& msg, const void* key, bool create_if_not_exist) const override;
};

template<typename K, typename V>
MapFieldDescriptorImpl<K, V>::IteratorImpl::IteratorImpl(const typename std::map<K, V>& c) :
    c_(c), it_(c.cbegin())
{
}

template<typename K, typename V>
bool MapFieldDescriptorImpl<K, V>::IteratorImpl::HasNext() const
{
    return it_ != c_.cend();
}

template<typename K, typename V>
void MapFieldDescriptorImpl<K, V>::IteratorImpl::Next()
{
    ++it_;
}

template<typename K, typename V>
const void* MapFieldDescriptorImpl<K, V>::IteratorImpl::GetKeyDataPtr() const
{
    return &it_->first;
}

template<typename K, typename V>
const void* MapFieldDescriptorImpl<K, V>::IteratorImpl::GetValueDataPtr() const
{
    return &it_->second;
}

template<typename K, typename V>
MapFieldDescriptorImpl<K, V>::MapFieldDescriptorImpl(std::string_view name,
        CppType cpp_type,
        size_t offset,
        CppType key_cpp_type,
        CppType value_cpp_type) :
    MapFieldDescriptor(name, cpp_type, offset, key_cpp_type, value_cpp_type)
{
}

template<typename K, typename V>
MapFieldDescriptorImpl<K, V>::MapFieldDescriptorImpl(std::string_view name,
        CppType cpp_type,
        size_t offset,
        CppType key_cpp_type,
        CppType value_cpp_type,
        const EnumDescriptor* enum_descriptor) :
    MapFieldDescriptor(name, cpp_type, offset, key_cpp_type, value_cpp_type, enum_descriptor)
{
}

template<typename K, typename V>
MapFieldDescriptorImpl<K, V>::MapFieldDescriptorImpl(std::string_view name,
        CppType cpp_type,
        size_t offset,
        CppType key_cpp_type,
        CppType value_cpp_type,
        const Descriptor* descriptor) :
    MapFieldDescriptor(name, cpp_type, offset, key_cpp_type, value_cpp_type, descriptor)
{
}

template<typename K, typename V>
MapFieldDescriptor::Iterator* MapFieldDescriptorImpl<K, V>::NewIterator(const Message& msg) const
{
    const std::map<K, V>* c = reinterpret_cast<const std::map<K, V>*>(reinterpret_cast<const char*>(&msg) + this->GetOffset());
    return new IteratorImpl(*c);
}

template<typename K, typename V>
void* MapFieldDescriptorImpl<K, V>::FindDataPtr(Message& msg, const void* key, bool create_if_not_exist) const
{
    std::map<K, V>* c = reinterpret_cast<std::map<K, V>*>(reinterpret_cast<char*>(&msg) + this->GetOffset());
    const K* p_key = reinterpret_cast<const K*>(key);
    auto it = c->find(*p_key);
    if (it != c->end()) return &it->second;
    if (create_if_not_exist) return &(*c)[*p_key];
    return nullptr;
}

template<typename K, typename V>
class UnorderedMapFieldDescriptorImpl : public MapFieldDescriptor
{
public:
    class IteratorImpl : public MapFieldDescriptor::Iterator
    {
    public:
        IteratorImpl(const typename std::unordered_map<K, V>& c);

        bool HasNext() const override;
        void Next() override;

    protected:
        const typename std::unordered_map<K, V>& c_;
        typename std::unordered_map<K, V>::const_iterator it_;

        const void* GetKeyDataPtr() const override;
        const void* GetValueDataPtr() const override;
    };

    UnorderedMapFieldDescriptorImpl(std::string_view name,
            CppType cpp_type,
            size_t offset,
            CppType key_cpp_type,
            CppType value_cpp_type);
    UnorderedMapFieldDescriptorImpl(std::string_view name,
            CppType cpp_type,
            size_t offset,
            CppType key_cpp_type,
            CppType value_cpp_type,
            const EnumDescriptor* enum_descriptor);
    UnorderedMapFieldDescriptorImpl(std::string_view name,
            CppType cpp_type,
            size_t offset,
            CppType key_cpp_type,
            CppType value_cpp_type,
            const Descriptor* descriptor);

    Iterator* NewIterator(const Message& msg) const override;

protected:
    void* FindDataPtr(Message& msg, const void* key, bool create_if_not_exist) const override;
};

template<typename K, typename V>
UnorderedMapFieldDescriptorImpl<K, V>::IteratorImpl::IteratorImpl(const typename std::unordered_map<K, V>& c) :
    c_(c), it_(c.cbegin())
{
}

template<typename K, typename V>
bool UnorderedMapFieldDescriptorImpl<K, V>::IteratorImpl::HasNext() const
{
    return it_ != c_.cend();
}

template<typename K, typename V>
void UnorderedMapFieldDescriptorImpl<K, V>::IteratorImpl::Next()
{
    ++it_;
}

template<typename K, typename V>
const void* UnorderedMapFieldDescriptorImpl<K, V>::IteratorImpl::GetKeyDataPtr() const
{
    return &it_->first;
}

template<typename K, typename V>
const void* UnorderedMapFieldDescriptorImpl<K, V>::IteratorImpl::GetValueDataPtr() const
{
    return &it_->second;
}

template<typename K, typename V>
UnorderedMapFieldDescriptorImpl<K, V>::UnorderedMapFieldDescriptorImpl(std::string_view name,
        CppType cpp_type,
        size_t offset,
        CppType key_cpp_type,
        CppType value_cpp_type) :
    MapFieldDescriptor(name, cpp_type, offset, key_cpp_type, value_cpp_type)
{
}

template<typename K, typename V>
UnorderedMapFieldDescriptorImpl<K, V>::UnorderedMapFieldDescriptorImpl(std::string_view name,
        CppType cpp_type,
        size_t offset,
        CppType key_cpp_type,
        CppType value_cpp_type,
        const EnumDescriptor* enum_descriptor) :
    MapFieldDescriptor(name, cpp_type, offset, key_cpp_type, value_cpp_type, enum_descriptor)
{
}

template<typename K, typename V>
UnorderedMapFieldDescriptorImpl<K, V>::UnorderedMapFieldDescriptorImpl(std::string_view name,
        CppType cpp_type,
        size_t offset,
        CppType key_cpp_type,
        CppType value_cpp_type,
        const Descriptor* descriptor) :
    MapFieldDescriptor(name, cpp_type, offset, key_cpp_type, value_cpp_type, descriptor)
{
}

template<typename K, typename V>
MapFieldDescriptor::Iterator* UnorderedMapFieldDescriptorImpl<K, V>::NewIterator(const Message& msg) const
{
    const std::unordered_map<K, V>* c = reinterpret_cast<const std::unordered_map<K, V>*>(reinterpret_cast<const char*>(&msg) + this->GetOffset());
    return new IteratorImpl(*c);
}

template<typename K, typename V>
void* UnorderedMapFieldDescriptorImpl<K, V>::FindDataPtr(Message& msg, const void* key, bool create_if_not_exist) const
{
    std::unordered_map<K, V>* c = reinterpret_cast<std::unordered_map<K, V>*>(reinterpret_cast<char*>(&msg) + this->GetOffset());
    const K* p_key = reinterpret_cast<const K*>(key);
    auto it = c->find(*p_key);
    if (it != c->end()) return &it->second;
    if (create_if_not_exist) return &(*c)[*p_key];
    return nullptr;
}

};
