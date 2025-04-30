#pragma once

#include <cstdint>
#include <string>
#include <memory>

#include <mrpc/error_code.mrpc.h>
#include <mrpc/message/message.h>

namespace mrpc
{

class Message;

class Callback : public std::enable_shared_from_this<Callback>
{
public:
    Callback() = default;
    virtual ~Callback() = default;

    virtual void Done(int32_t ret, const std::string& rsp_data) = 0;
};

template<typename T>
    requires(std::is_base_of_v<Message, T>)
class AsyncCallback : public Callback
{
public:
    virtual void Done(int32_t ret, const T& rsp) = 0;

    void Done(int32_t ret, const std::string& rsp_data) override
    {
        T rsp;
        if (ret == 0)
        {
            if (!rsp.ParseFromString(rsp_data))
            {
                ret = ERROR_INVALID_METHOD_RESPONSE_DATA;
            }
        }
        Done(ret, rsp);
    }
};

using CallbackPtr = std::shared_ptr<Callback>;

}
