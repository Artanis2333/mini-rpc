#pragma once

#include <memory>

#include <mrpc/service/context.h>
#include <mrpc/util/noncopyable.h>

namespace mrpc
{

class NetworkClientImpl;

class NetworkClient : private NonCopyable
{
public:
    NetworkClient();
    ~NetworkClient();

    void Start();
    void Stop();

    void SendRequest(const std::shared_ptr<ServiceStubContext>& context);

private:
    NetworkClientImpl* impl_;
};

}
