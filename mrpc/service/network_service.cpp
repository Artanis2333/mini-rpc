#include <cassert>
#include <cstring>
#include <atomic>
#include <unordered_map>
#include <uv.h>

#include <mrpc/error_code.mrpc.h>
#include <mrpc/service/network_service.h>
#include <mrpc/service/service_bridge.h>
#include <mrpc/service/application_config.mrpc.h>
#include <mrpc/util/log.h>
#include <mrpc/util/thread_safe_queue.h>

namespace mrpc
{

struct write_req_t
{
    uv_write_t req;
    uv_buf_t buf;
};

static void GetPeerTcpAddrName(uv_tcp_t* handle, std::string& host, uint32_t& port)
{
    struct sockaddr_storage rawname;
    int namelen = sizeof(rawname);
    if (uv_tcp_getpeername(handle, (struct sockaddr*)&rawname, &namelen) != 0)
    {
        return;
    }

    char ip[INET6_ADDRSTRLEN];
    if (rawname.ss_family == AF_INET)
    {
        struct sockaddr_in* addr_v4 = (struct sockaddr_in*)&rawname;
        uv_ip4_name(addr_v4, ip, sizeof(ip));
        port = ntohs(addr_v4->sin_port);
    }
    else if (rawname.ss_family == AF_INET6)
    {
        struct sockaddr_in6* addr_v6 = (struct sockaddr_in6*)&rawname;
        uv_ip6_name(addr_v6, ip, sizeof(ip));
        port = ntohs(addr_v6->sin6_port);
    }

    host = ip;
}

struct NetworkServiceTcpConnection
{
    NetworkServiceTcpConnection();
    ~NetworkServiceTcpConnection() = default;

    uint64_t conn_id = 0;
    std::string host;
    uint32_t port = 0;
    Protocol protocol;
    bool close = false;
    std::string buffer;

    // uv fields
    uv_tcp_t handle;

    void Close(uv_close_cb cb);
};

NetworkServiceTcpConnection::NetworkServiceTcpConnection()
{
    uv_handle_set_data((uv_handle_t*)&handle, this);
}

void NetworkServiceTcpConnection::Close(uv_close_cb cb)
{
    close = true;
    uv_close((uv_handle_t*)&handle, cb);
}

class NetworkServiceImpl
{
public:
    NetworkServiceImpl();
    ~NetworkServiceImpl();

    void SetBridge(ServiceBridge* bridge);
    void SetAllProtocol(const std::vector<Protocol>& all_protocol);

    int BindTcpAddr(const std::string& host, uint32_t port, uint32_t backlog);

    void Start();
    void Stop();

    void SendResponse(const std::shared_ptr<ServiceContext>& context);

private:
    static void OnNewTcpConnection(uv_stream_t* server, int status);
    static void OnCloseTcpConnection(uv_handle_t *handle);
    static void OnAllocBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
    static void OnRead(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf);
    static void OnRequestWrite(uv_async_t* handle);
    static void OnWrite(uv_write_t *req, int status);

    std::atomic<bool> running_ = false;
    std::string host_;
    uint32_t port_ = 0;
    ServiceBridge* bridge_ = nullptr;
    std::vector<Protocol> all_protocol_;
    std::unordered_map<uint64_t, std::unique_ptr<NetworkServiceTcpConnection>> id2conn_;
    ThreadSafeQueue<std::shared_ptr<ServiceContext>> queue_;

    static inline uint64_t next_conn_id_ = 0;

    // uv fields
    uv_loop_t loop_;
    uv_tcp_t tcp_server_;
    uv_async_t async_;

};

NetworkServiceImpl::NetworkServiceImpl()
{
    int ret = uv_loop_init(&loop_);
    assert(ret == 0);
    uv_loop_set_data(&loop_, this);

    ret = uv_async_init(&loop_, &async_, OnRequestWrite);
    assert(ret == 0);
}

NetworkServiceImpl::~NetworkServiceImpl()
{
    int ret = uv_loop_close(&loop_);
    assert(ret == 0);
}

void NetworkServiceImpl::SetBridge(ServiceBridge* bridge)
{
    bridge_ = bridge;
}

void NetworkServiceImpl::SetAllProtocol(const std::vector<Protocol>& all_protocol)
{
    all_protocol_ = all_protocol;
}

int NetworkServiceImpl::BindTcpAddr(const std::string& host, uint32_t port, uint32_t backlog)
{
    int ret = 0;

    ret = uv_tcp_init(&loop_, &tcp_server_);
    assert(ret == 0);

    struct sockaddr_in addr_v4;
    struct sockaddr_in6 addr_v6;
    if (uv_ip4_addr(host.c_str(), port, &addr_v4) == 0)
    {
        ret = uv_tcp_bind(&tcp_server_, (const struct sockaddr*)&addr_v4, 0);
        assert(ret == 0);
    }
    else if (uv_ip6_addr(host.c_str(), port, &addr_v6) == 0)
    {
        ret = uv_tcp_bind(&tcp_server_, (const struct sockaddr*)&addr_v6, 0);
        assert(ret == 0);
    }
    else
    {
        MRPC_LOG_ERROR("Invalid server address {}:{}", host, port);
        return ERROR_INITIALIZATION_FAILED;
    }

    ret = uv_tcp_nodelay(&tcp_server_, 1);
    assert(ret == 0);

    ret = uv_listen((uv_stream_t*)&tcp_server_, backlog, OnNewTcpConnection);
    if (ret != 0)
    {
        MRPC_LOG_ERROR("Server listen on address {}:{} error, {}", host, port, uv_strerror(ret));
        return ERROR_INITIALIZATION_FAILED;
    }

    host_ = host;
    port_ = port;
    MRPC_LOG_INFO("Server listen on address {}:{} success...", host_, port_);
    return 0;
}

void NetworkServiceImpl::OnNewTcpConnection(uv_stream_t* server, int status)
{
    if (status < 0)
    {
        MRPC_LOG_DEBUG("New connection error, {}", uv_strerror(status));
        return;
    }

    NetworkServiceTcpConnection* conn = new NetworkServiceTcpConnection();
    NetworkServiceImpl* service = (NetworkServiceImpl*)uv_loop_get_data(uv_handle_get_loop((uv_handle_t*)server));
    int ret = uv_tcp_init(&service->loop_, &conn->handle);
    assert(ret == 0);

    if (uv_accept(server, (uv_stream_t*)&conn->handle) == 0)
    {
        conn->conn_id = ++next_conn_id_;
        GetPeerTcpAddrName(&conn->handle, conn->host, conn->port);
        if (service->all_protocol_.size() == 1)
        {
            conn->protocol = service->all_protocol_[0];
        }
        service->id2conn_.emplace(conn->conn_id, conn);

        MRPC_LOG_DEBUG("Accept new connection, conn id {}, self addr {}:{}, peer addr {}:{}", conn->conn_id, service->host_, service->port_, conn->host, conn->port);

        ret = uv_read_start((uv_stream_t*)&conn->handle, OnAllocBuffer, OnRead);
        assert(ret == 0);
    }
    else
    {
        conn->Close(OnCloseTcpConnection);
    }
}

void NetworkServiceImpl::OnCloseTcpConnection(uv_handle_t* handle)
{
    NetworkServiceImpl* service = (NetworkServiceImpl*)uv_loop_get_data(uv_handle_get_loop(handle));
    NetworkServiceTcpConnection* conn = (NetworkServiceTcpConnection*)uv_handle_get_data(handle);
    MRPC_LOG_DEBUG("Close connection, conn id {}, self addr {}:{}, peer addr {}:{}", conn->conn_id, service->host_, service->port_, conn->host, conn->port);
    service->id2conn_.erase(conn->conn_id);
}

void NetworkServiceImpl::OnAllocBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
    (void)handle;
    buf->base = (char*)malloc(suggested_size);
    buf->len = suggested_size;
}

void NetworkServiceImpl::OnRead(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf)
{
    NetworkServiceImpl* service = (NetworkServiceImpl*)uv_loop_get_data(uv_handle_get_loop((uv_handle_t*)handle));
    NetworkServiceTcpConnection* conn = (NetworkServiceTcpConnection*)uv_handle_get_data((uv_handle_t*)handle);
    if (nread > 0)
    {
        conn->buffer.append(buf->base, nread);
        MRPC_LOG_TRACE("Received {} bytes", nread);

        const char* begin = conn->buffer.c_str();
        const char* end = conn->buffer.c_str() + conn->buffer.size();
        if (conn->protocol.parse == nullptr)
        {
            bool has_error = false;
            std::shared_ptr<ServiceContext> context;
            for (const auto& protocol : service->all_protocol_)
            {
                if (protocol.parse(begin, end - begin, false, has_error, context))
                {
                    conn->protocol = protocol;

                    context->conn_id = conn->conn_id;
                    context->host = conn->host;
                    context->port = conn->port;
                    context->protocol = conn->protocol;
                    service->bridge_->DispatchMessage(context);
                    break;
                }
                else if (has_error)
                {
                    conn->Close(OnCloseTcpConnection);
                    break;
                }
            }
        }

        if (conn->protocol.parse != nullptr)
        {
            while (begin < end)
            {
                bool has_error = false;
                std::shared_ptr<ServiceContext> context;
                if (conn->protocol.parse(begin, end - begin, true, has_error, context))
                {
                    context->conn_id = conn->conn_id;
                    context->host = conn->host;
                    context->port = conn->port;
                    context->protocol = conn->protocol;
                    service->bridge_->DispatchMessage(context);
                }
                else if (has_error)
                {
                    conn->Close(OnCloseTcpConnection);
                    break;
                }
                else
                {
                    break;
                }
            }
        }

        if (begin < end)
        {
            if (begin != conn->buffer.c_str())
            {
                conn->buffer.assign(begin, end - begin);
            }
        }
        else
        {
            conn->buffer.clear();
        }
    }
    else if (nread < 0)
    {
        if (nread != UV_EOF)
        {
            MRPC_LOG_DEBUG("Read error, {}", uv_err_name(nread));
        }
        conn->Close(OnCloseTcpConnection);
    }

    free(buf->base);
}

void NetworkServiceImpl::OnRequestWrite(uv_async_t* handle)
{
    NetworkServiceImpl* service = (NetworkServiceImpl*)uv_loop_get_data(uv_handle_get_loop((uv_handle_t*)handle));
    std::shared_ptr<ServiceContext> context;
    while (service->queue_.try_pop(context))
    {
        NetworkServiceTcpConnection* conn = nullptr;
        auto it = service->id2conn_.find(context->conn_id);
        if (it != service->id2conn_.end())
        {
            conn = it->second.get();
        }
        if (conn == nullptr || conn->close)
        {
            MRPC_LOG_DEBUG("Discard response, conn id {}, data length {}", context->conn_id, context->response.length());
            continue;
        }

        if (!context->response.empty())
        {
            size_t length = context->response.length();
            write_req_t* write_req = (write_req_t*)malloc(sizeof(write_req_t));
            write_req->buf = uv_buf_init((char*)malloc(length), length);
            memcpy(write_req->buf.base, context->response.c_str(), length);
            int ret = uv_write((uv_write_t*)write_req, (uv_stream_t*)&conn->handle, &write_req->buf, 1, OnWrite);
            assert(ret == 0);
            MRPC_LOG_TRACE("Send response, conn id {}, data length {}", context->conn_id, context->response.length());
        }

        if (context->close_connection)
        {
            conn->Close(OnCloseTcpConnection);
        }

        context.reset();
    }
}

void NetworkServiceImpl::OnWrite(uv_write_t* req, int status)
{
    if (status)
    {
        MRPC_LOG_DEBUG("Write error, {}", uv_strerror(status));
    }

    write_req_t* write_req = (write_req_t*)req;
    free(write_req->buf.base);
    free(write_req);
}

void NetworkServiceImpl::Start()
{
    running_ = true;
    while (running_)
    {
        uv_run(&loop_, UV_RUN_DEFAULT);
    }
}

void NetworkServiceImpl::Stop()
{
    running_ = false;
    uv_stop(&loop_);
}

void NetworkServiceImpl::SendResponse(const std::shared_ptr<ServiceContext>& context)
{
    queue_.push(context);
    int ret = uv_async_send(&async_);
    assert(ret == 0);
}

NetworkService::NetworkService() : impl_(new NetworkServiceImpl())
{
}

NetworkService::~NetworkService()
{
    delete impl_;
}

void NetworkService::SetBridge(ServiceBridge* bridge)
{
    impl_->SetBridge(bridge);
}

void NetworkService::SetAllProtocol(const std::vector<Protocol>& all_protocol)
{
    impl_->SetAllProtocol(all_protocol);
}

int NetworkService::Bind(const NetworkConfig& config)
{
    switch (config.protocol)
    {
        case tcp:
            return impl_->BindTcpAddr(config.host, config.port, config.tcp_backlog);
            break;
        default:
            break;
    }

    MRPC_LOG_ERROR("Invalid protocol {}", NetworkProtocol_Name(config.protocol));
    return ERROR_INITIALIZATION_FAILED;
}

void NetworkService::Start()
{
    impl_->Start();
}

void NetworkService::Stop()
{
    impl_->Stop();
}

void NetworkService::SendResponse(const std::shared_ptr<ServiceContext>& context)
{
    impl_->SendResponse(context);
}

}
