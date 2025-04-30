#include <mrpc/service/global_proxy.h>
#include <mrpc/util/log.h>

namespace mrpc
{

std::map<std::string, Endpoint> GlobalProxy::local_name2endpoint_;
std::atomic<uint64_t> GlobalProxy::next_stub_id_ = 0;
NetworkClient GlobalProxy::network_;
std::thread GlobalProxy::network_thread_;

void GlobalProxy::RegisterLocalEndpoint(const std::string& name, const Endpoint& endpoint)
{
    local_name2endpoint_[name] = endpoint;
}

void GlobalProxy::Start()
{
    network_thread_ = std::thread(NetworkThreadFunc, std::ref(network_));
}

void GlobalProxy::Stop()
{
    network_.Stop();
    network_thread_.join();
}

void GlobalProxy::SendRequest(const std::shared_ptr<ServiceStubContext>& context)
{
    network_.SendRequest(context);
}

const Endpoint* GlobalProxy::FindLocalEndpoint(const std::string& name)
{
    auto it = local_name2endpoint_.find(name);
    if (it != local_name2endpoint_.end())
    {
        return &it->second;
    }
    return nullptr;
}

void GlobalProxy::InitServiceStub(const std::shared_ptr<ServiceStub>& stub, const Endpoint& endpoint)
{
    stub->stub_id_ = ++next_stub_id_;
    stub->endpoint_ = endpoint;
    const Protocol* protocol = GlobalFindProtocol("mrpc");//TODO
    if (protocol != nullptr)
    {
        stub->protocol_ = *protocol;
    }
}

void GlobalProxy::NetworkThreadFunc(NetworkClient& network)
{
    MRPC_LOG_INFO("Thread start");//, id {}", std::this_thread::get_id());

    network.Start();
}

}
