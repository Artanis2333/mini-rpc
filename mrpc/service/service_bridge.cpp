#include <mrpc/error_code.mrpc.h>
#include <mrpc/service/service_bridge.h>
#include <mrpc/service/service_factory.h>
#include <mrpc/util/log.h>
#include <mrpc/util/string.h>

namespace mrpc
{

int ServiceBridge::Initialize(const ServiceFactory& factory, const ServiceConfig& config)
{
    int ret = 0;
    config_ = config;
    network_.SetBridge(this);

    std::vector<Protocol> all_protocol;
    std::vector<std::string> all_protocol_name = String::Split(config_.protocol, ',');
    for (const auto& protocol_name : all_protocol_name)
    {
        const Protocol* protocol = GlobalFindProtocol(protocol_name);
        if (protocol == nullptr)
        {
            MRPC_LOG_ERROR("Protocol {} is not registered.", protocol_name);
            return ERROR_INITIALIZATION_FAILED;
        }
        if (!protocol->IsServer())
        {
            MRPC_LOG_ERROR("Protocol {} dose not implement all server functions.", protocol_name);
            return ERROR_INITIALIZATION_FAILED;
        }
        all_protocol.push_back(*protocol);
    }
    network_.SetAllProtocol(all_protocol);

    ret = network_.Bind(config_.network);
    if (ret != 0)
    {
        return ret;
    }

    for (uint32_t i = 0; i < config_.thread_num; ++i)
    {
        Service* service = factory.NewService(config_.name);
        if (service == nullptr)
        {
            MRPC_LOG_ERROR("Service {} is not registered.", config_.name);
            return ERROR_INITIALIZATION_FAILED;
        }

        service_.emplace_back(service);
        service_queues_.emplace_back(new ContextPtrQueue());
    }

    return 0;
}

void ServiceBridge::Start()
{
    running_ = true;
    for (uint32_t i = 0; i < config_.thread_num; ++i)
    {
        service_thread_.emplace_back(ServiceThreadFunc, std::ref(running_), std::ref(network_), std::ref(*service_[i]), std::ref(*service_queues_[i]));
    }
    network_thread_ = std::thread(NetworkThreadFunc, std::ref(network_));
}

void ServiceBridge::Stop()
{
    running_ = false;
    network_.Stop();

    network_thread_.join();
    for (uint32_t i = 0; i < config_.thread_num; ++i)
    {
        service_thread_[i].join();
    }
}

void ServiceBridge::Finalize()
{
}

void ServiceBridge::DispatchMessage(const std::shared_ptr<ServiceContext>& context)
{
    size_t index = 0;
    if (context->param.has_thread_hash_code)
    {
        index = context->param.thread_hash_code % config_.thread_num;
    }
    else
    {
        index = context->conn_id % config_.thread_num;
    }

    service_queues_[index]->push(context);
}

void ServiceBridge::NetworkThreadFunc(NetworkService& network)
{
    MRPC_LOG_INFO("Thread start");//, id {}", std::this_thread::get_id());

    network.Start();
}

struct ServiceThreadVisitor
{
    ServiceThreadVisitor(NetworkService& n, Service& s) : network(n), service(s) {}

    void operator()(const std::shared_ptr<ServiceContext>& context);
    void operator()(const std::shared_ptr<ServiceStubContext>& context);

    NetworkService& network;
    Service& service;
};

void ServiceThreadVisitor::operator()(const std::shared_ptr<ServiceContext>& context)
{
    context->protocol.handle_request(service, context);
    if ((context->param.need_response && context->send_response) || context->close_connection)
    {
        network.SendResponse(context);
    }
}

void ServiceThreadVisitor::operator()(const std::shared_ptr<ServiceStubContext>& context)
{
    context->callback->Done(context->ret, context->response_payload);
}

void ServiceBridge::ServiceThreadFunc(std::atomic<bool>& running, NetworkService& network, Service& service, ContextPtrQueue& queue)
{
    SetCurrentThreadContextPtrQueue(&queue);
    MRPC_LOG_INFO("Thread start");//, id {}", std::this_thread::get_id());

    ServiceThreadVisitor visitor(network, service);
    std::chrono::milliseconds timeout(20);
    while (running)
    {
        ContextPtrWrapper context_wrapper;
        if (!queue.wait_and_pop(context_wrapper, timeout))
        {
            continue;
        }

        std::visit(visitor, context_wrapper);
    }
}

}
