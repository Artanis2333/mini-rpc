#include <mrpc/message/message.h>

namespace mrpc
{

Message* Message::New() const
{
    return nullptr;
}

void Message::CopyFrom(const Message& /*msg*/)
{
}

const Descriptor* Message::GetDescriptor() const
{
    return nullptr;
}

void Message::SerializeToString(std::string& s, bool skip_default /*= true*/)
{
    size_t size = ByteSize(skip_default);
    s.clear();
    if (s.capacity() < size)
    {
        s.reserve(size);
    }

    if (skip_default)
    {
        SerializeToStringSkipDefault(s);
    }
    else
    {
        SerializeToStringNotSkipDefault(s);
    }
}

bool Message::ParseFromString(std::string_view s)
{
    return ParseFromBytes(reinterpret_cast<const uint8_t*>(s.data()), reinterpret_cast<const uint8_t*>(s.data() + s.size()));
}

}
