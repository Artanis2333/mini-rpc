#pragma once

#include <cstdint>
#include <string>
#include <memory>

#include <mrpc/util/noncopyable.h>

namespace mrpc
{

class Message;
class Descriptor;

class Service : private NonCopyable
{
public:
    Service() = default;
    virtual ~Service() = default;

    // name based protocol
    virtual const Descriptor* GetRequestDescriptor(const std::string& method_name) const = 0;
    virtual const Descriptor* GetResponseDescriptor(const std::string& method_name) const = 0;
    virtual int32_t CallMethod(const std::string& method_name, const Message& req, Message& rsp) = 0;

    // name hash based protocol
    virtual int32_t CallMethod(uint32_t method_code, const std::string& req_data, std::string& rsp_data) = 0;
};

}
