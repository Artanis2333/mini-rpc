#pragma once

#include <stdint.h>
#include <string>
#include <string_view>

namespace mrpc
{

class EnumDescriptor;
class Descriptor;
class Message;

template<bool skip_default>
inline void Serialize(std::string&, Message&);

class Message
{
public:
    Message() = default;
    virtual ~Message() = default;

    // Name.
    virtual std::string_view GetName() const = 0;
    virtual std::string_view GetFullName() const = 0;

    // Reflection.
    virtual Message* New() const;
    virtual void CopyFrom(const Message& msg);
    virtual const Descriptor* GetDescriptor() const;

    // Clear data.
    virtual void Clear() = 0;

    // Byte size.
    inline size_t GetCachedSize() const { return cached_size_; }

    inline size_t ByteSize(bool skip_default = true)
    {
        if (skip_default) return ByteSizeSkipDefault();
        return ByteSizeNotSkipDefault();
    }

    // Serialize.
    void SerializeToString(std::string& s, bool skip_default = true);

    // Parse.
    bool ParseFromString(std::string_view s);

protected:
    size_t cached_size_ = 0;

    virtual size_t ByteSizeSkipDefault() = 0;
    virtual size_t ByteSizeNotSkipDefault() = 0;

    virtual void SerializeToStringSkipDefault(std::string& s) = 0;
    virtual void SerializeToStringNotSkipDefault(std::string& s) = 0;

    virtual bool ParseFromBytes(const uint8_t* begin, const uint8_t* const end) = 0;

    friend void Serialize<false>(std::string&, Message&);
    friend void Serialize<true>(std::string&, Message&);
    friend bool Parse(Message&, const uint8_t*&, const uint8_t* const);
};

template<typename T>
class ReflectableMessage : public Message
{
public:
    Message* New() const override;
    void CopyFrom(const Message& msg) override;
};

template<typename T>
Message* ReflectableMessage<T>::New() const
{
    return new T;
}

template<typename T>
void ReflectableMessage<T>::CopyFrom(const Message& msg)
{
    const T* src = dynamic_cast<const T*>(&msg);
    if (src == nullptr) return;

    T* dst = dynamic_cast<T*>(this);
    if (dst == nullptr) return;

    *dst = *src;
}

}
