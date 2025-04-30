#pragma once

#include <memory>
#include <vector>

#include <mrpc/service/context.h>
#include <mrpc/service/protocol.h>
#include <mrpc/util/noncopyable.h>

namespace mrpc
{

class NetworkConfig;
class ServiceBridge;
class NetworkServiceImpl;

class NetworkService : private NonCopyable
{
public:
    NetworkService();
    ~NetworkService();

    void SetBridge(ServiceBridge* bridge);
    void SetAllProtocol(const std::vector<Protocol>& all_protocol);

    int Bind(const NetworkConfig& config);

    void Start();
    void Stop();

    void SendResponse(const std::shared_ptr<ServiceContext>& context);

private:
    NetworkServiceImpl* impl_;
};

}
