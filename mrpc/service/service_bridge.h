#pragma once

#include <memory>
#include <vector>
#include <thread>

#include <mrpc/service/application_config.mrpc.h>
#include <mrpc/service/context.h>
#include <mrpc/service/network_service.h>
#include <mrpc/service/service.h>
#include <mrpc/service/service_factory.h>
#include <mrpc/util/noncopyable.h>
#include <mrpc/util/thread_safe_queue.h>

namespace mrpc
{

class ServiceBridge final : private NonCopyable
{
public:
    ServiceBridge() = default;
    ~ServiceBridge() = default;

    int Initialize(const ServiceFactory& factory, const ServiceConfig& config);
    void Start();
    void Stop();
    void Finalize();


    void DispatchMessage(const std::shared_ptr<ServiceContext>& context);

private:
    static void NetworkThreadFunc(NetworkService& network);
    static void ServiceThreadFunc(std::atomic<bool>& running, NetworkService& network, Service& service, ContextPtrQueue& queue);

    ServiceConfig config_;
    std::atomic<bool> running_ = false;

    NetworkService network_;
    std::thread network_thread_;

    std::vector<std::unique_ptr<Service>> service_;
    std::vector<std::unique_ptr<ContextPtrQueue>> service_queues_;
    std::vector<std::thread> service_thread_;
};

}
