#pragma once

#include <memory>
#include <mutex>
#include <map>
#include <vector>
#include <thread>

#include <mrpc/service/network_client.h>
#include <mrpc/service/service_stub.h>

namespace mrpc
{

template<typename T>
    requires(std::is_base_of_v<ServiceStub, T>)
class ServiceProxy
{
public:
    void SendRequest(const std::shared_ptr<ServiceStubContext>& context);
private:
    std::vector<std::shared_ptr<T>> stubs_;
};

class GlobalProxy
{
public:
    static void RegisterLocalEndpoint(const std::string& name, const Endpoint& endpoint);

    static void Start();
    static void Stop();

    template<typename T>
        requires(std::is_base_of_v<ServiceStub, T>)
    static std::shared_ptr<T> FindServiceStub(const std::string& name);

    static void SendRequest(const std::shared_ptr<ServiceStubContext>& context);

private:
    static const Endpoint* FindLocalEndpoint(const std::string& name);
    static void InitServiceStub(const std::shared_ptr<ServiceStub>& stub, const Endpoint& endpoint);

    static void NetworkThreadFunc(NetworkClient& network);

    static std::map<std::string, Endpoint> local_name2endpoint_;
    static std::atomic<uint64_t> next_stub_id_;

    static NetworkClient network_;
    static std::thread network_thread_;
};


template<typename T>
    requires(std::is_base_of_v<ServiceStub, T>)
std::shared_ptr<T> GlobalProxy::FindServiceStub(const std::string& name)
{
    const Endpoint* endpoint = FindLocalEndpoint(name);
    if (endpoint == nullptr)
    {
        return nullptr;
    }

    std::shared_ptr<T> stub = std::make_shared<T>();
    InitServiceStub(stub, *endpoint);
    return stub;
}

}
