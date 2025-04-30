#include <cassert>
#include <cstring>
#include <atomic>
#include <unordered_map>
#include <uv.h>

#include <mrpc/error_code.mrpc.h>
#include <mrpc/service/network_client.h>
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

static void GetSelfTcpAddrName(uv_tcp_t* handle, std::string& host, uint32_t& port)
{
    struct sockaddr_storage rawname;
    int namelen = sizeof(rawname);
    if (uv_tcp_getsockname(handle, (struct sockaddr*)&rawname, &namelen) != 0)
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

struct NetworkClientTcpConnection
{
    NetworkClientTcpConnection();
    ~NetworkClientTcpConnection() = default;

    uint64_t stub_id = 0;
    std::string self_host;
    uint32_t self_port = 0;
    std::string peer_host;
    uint32_t peer_port = 0;
    Protocol protocol;
    bool close = false;
    std::string buffer;
    std::vector<std::string> send_buffer;

    // uv fields
    uv_connect_t connect_handle;
    uv_tcp_t handle;

    void FlushBuffer();

    void Write(const std::string& packet);
    static void OnWrite(uv_write_t* req, int status);

    void Close(uv_close_cb cb);
};

NetworkClientTcpConnection::NetworkClientTcpConnection()
{
    uv_handle_set_data((uv_handle_t*)&handle, this);
}

void NetworkClientTcpConnection::FlushBuffer()
{
    for (auto& packet : send_buffer)
    {
        Write(packet);
    }
    send_buffer.clear();
}

void NetworkClientTcpConnection::Write(const std::string& packet)
{
    size_t length = packet.length();
    write_req_t* write_req = (write_req_t*)malloc(sizeof(write_req_t));
    write_req->buf = uv_buf_init((char*)malloc(length), length);
    memcpy(write_req->buf.base, packet.c_str(), length);
    int ret = uv_write((uv_write_t*)write_req, (uv_stream_t*)&handle, &write_req->buf, 1, OnWrite);
    assert(ret == 0);
    MRPC_LOG_DEBUG("Send request, stub id {}, data length {}", stub_id, length);
}

void NetworkClientTcpConnection::OnWrite(uv_write_t* req, int status)
{
    if (status)
    {
        MRPC_LOG_ERROR("Write error, {}", uv_strerror(status));
    }

    write_req_t* write_req = (write_req_t*)req;
    free(write_req->buf.base);
    free(write_req);
}

void NetworkClientTcpConnection::Close(uv_close_cb cb)
{
    close = true;
    uv_close((uv_handle_t*)&handle, cb);
}

class NetworkClientImpl
{
public:
    NetworkClientImpl();
    ~NetworkClientImpl();

    int Connect(uint64_t stub_id, const Protocol& protocol, const Endpoint& endpoint);
    int ConnectToTcpAddr(uint64_t stub_id, const Protocol& protocol, const std::string& host, uint32_t port);

    void Start();
    void Stop();

    void SendRequest(const std::shared_ptr<ServiceStubContext>& context);

private:
    static void OnTcpConnect(uv_connect_t* req, int status);
    static void OnCloseTcpConnection(uv_handle_t *handle);
    static void OnAllocBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
    static void OnRead(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf);
    static void OnRequestWrite(uv_async_t* handle);
    static void OnWrite(uv_write_t *req, int status);

    std::atomic<bool> running_ = false;
    std::unordered_map<uint64_t, std::unique_ptr<NetworkClientTcpConnection>> id2conn_;
    std::unordered_map<uint64_t, std::shared_ptr<ServiceStubContext>> id2context_;
    ThreadSafeQueue<std::shared_ptr<ServiceStubContext>> queue_;

    // uv fields
    uv_loop_t loop_;
    uv_async_t async_;

};

NetworkClientImpl::NetworkClientImpl()
{
    int ret = uv_loop_init(&loop_);
    assert(ret == 0);
    uv_loop_set_data(&loop_, this);

    ret = uv_async_init(&loop_, &async_, OnRequestWrite);
    assert(ret == 0);
}

NetworkClientImpl::~NetworkClientImpl()
{
    uv_loop_close(&loop_);
    /*TODO
    int ret = uv_loop_close(&loop_);
    assert(ret == 0);
    */
}

int NetworkClientImpl::Connect(uint64_t stub_id, const Protocol& protocol, const Endpoint& endpoint)
{
    switch (endpoint.protocol)
    {
        case tcp:
            return ConnectToTcpAddr(stub_id, protocol, endpoint.host, endpoint.port);
            break;
        default:
            break;
    }

    MRPC_LOG_ERROR("Invalid protocol, stub id {}", stub_id);
    return ERROR_INITIALIZATION_FAILED;
}

int NetworkClientImpl::ConnectToTcpAddr(uint64_t stub_id, const Protocol& protocol, const std::string& host, uint32_t port)
{
    int ret = 0;
    NetworkClientTcpConnection* conn = new NetworkClientTcpConnection();
    conn->stub_id = stub_id;
    conn->peer_host = host;
    conn->peer_port = port;
    conn->protocol = protocol;

    ret = uv_tcp_init(&loop_, &conn->handle);
    assert(ret == 0);
    ret = uv_tcp_nodelay(&conn->handle, 1);
    assert(ret == 0);

    struct sockaddr_in addr_v4;
    struct sockaddr_in6 addr_v6;
    if (uv_ip4_addr(host.c_str(), port, &addr_v4) == 0)
    {
        ret = uv_tcp_connect(&conn->connect_handle, &conn->handle, (const struct sockaddr*)&addr_v4, OnTcpConnect);
        assert(ret == 0);
    }
    else if (uv_ip6_addr(host.c_str(), port, &addr_v6) == 0)
    {
        ret = uv_tcp_connect(&conn->connect_handle, &conn->handle, (const struct sockaddr*)&addr_v6, OnTcpConnect);
        assert(ret == 0);
    }
    else
    {
        MRPC_LOG_ERROR("Invalid address {}:{}", host, port);
        delete conn;
        return ERROR_INITIALIZATION_FAILED;
    }

    id2conn_.emplace(stub_id, conn);
    MRPC_LOG_DEBUG("Connecting to address {}:{} success...", host, port);
    return 0;
}

void NetworkClientImpl::OnTcpConnect(uv_connect_t* req, int status)
{
    uv_stream_t* connection = req->handle;
    NetworkClientTcpConnection* conn = (NetworkClientTcpConnection*)uv_handle_get_data((uv_handle_t*)connection);
    if (status < 0)
    {
        conn->Close(OnCloseTcpConnection);
        MRPC_LOG_ERROR("New connection for stub {} error, {}", conn->stub_id, uv_strerror(status));
        return;
    }

    GetSelfTcpAddrName(&conn->handle, conn->self_host, conn->self_port);
    MRPC_LOG_DEBUG("New connection, stub id {}, self addr {}:{}, peer addr {}:{}", conn->stub_id, conn->self_host, conn->self_port, conn->peer_host, conn->peer_port);

    int ret = uv_read_start((uv_stream_t*)&conn->handle, OnAllocBuffer, OnRead);
    assert(ret == 0);

    conn->FlushBuffer();
}

void NetworkClientImpl::OnCloseTcpConnection(uv_handle_t* handle)
{
    NetworkClientImpl* client = (NetworkClientImpl*)uv_loop_get_data(uv_handle_get_loop(handle));
    NetworkClientTcpConnection* conn = (NetworkClientTcpConnection*)uv_handle_get_data(handle);
    MRPC_LOG_DEBUG("Close connection, stub id {}, self addr {}:{}, peer addr {}:{}", conn->stub_id, conn->self_host, conn->self_port, conn->peer_host, conn->peer_port);
    client->id2conn_.erase(conn->stub_id);
}

void NetworkClientImpl::OnAllocBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
    (void)handle;
    buf->base = (char*)malloc(suggested_size);
    buf->len = suggested_size;
}

void NetworkClientImpl::OnRead(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf)
{
    NetworkClientImpl* client = (NetworkClientImpl*)uv_loop_get_data(uv_handle_get_loop((uv_handle_t*)handle));
    NetworkClientTcpConnection* conn = (NetworkClientTcpConnection*)uv_handle_get_data((uv_handle_t*)handle);
    if (nread > 0)
    {
        conn->buffer.append(buf->base, nread);
        MRPC_LOG_TRACE("Received {} bytes", nread);

        const char* begin = conn->buffer.c_str();
        const char* end = conn->buffer.c_str() + conn->buffer.size();
        while (begin < end)
        {
            bool has_error = false;
            uint64_t seq_id = 0;
            int32_t ret_value = 0;
            std::string response_payload;
            if (conn->protocol.handle_response(begin, end - begin, has_error, seq_id, ret_value, response_payload))
            {
                std::shared_ptr<ServiceStubContext> context;
                auto it = client->id2context_.find(seq_id);
                if (it != client->id2context_.end())
                {
                    context = it->second;
                    client->id2context_.erase(it);
                }

                if (!context)
                {
                    continue;
                }

                context->ret = ret_value;
                context->response_payload = std::move(response_payload);
                if (context->queue != nullptr)
                {
                    ((ContextPtrQueue*)context->queue)->push(context);
                }
                else if (context->notifier)
                {
                    context->notifier->cv.notify_one();
                }
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
            MRPC_LOG_ERROR("Read error, {}", uv_err_name(nread));
        }
        conn->Close(OnCloseTcpConnection);
    }

    free(buf->base);
}

void NetworkClientImpl::OnRequestWrite(uv_async_t* handle)
{
    NetworkClientImpl* client = (NetworkClientImpl*)uv_loop_get_data(uv_handle_get_loop((uv_handle_t*)handle));
    std::shared_ptr<ServiceStubContext> context;
    while (client->queue_.try_pop(context))
    {
        if (context->param.need_response)
        {
            client->id2context_[context->seq_id] = context;
        }

        NetworkClientTcpConnection* conn = nullptr;
        auto it = client->id2conn_.find(context->stub_id);
        if (it != client->id2conn_.end())
        {
            conn = it->second.get();
        }
        if (conn != nullptr && conn->close)
        {
            MRPC_LOG_DEBUG("Discard request, stub id {}, data length {}", context->stub_id, context->request.length());
            continue;
        }

        if (conn == nullptr)
        {
            int ret = client->Connect(context->stub_id, context->protocol, context->endpoint);
            if (ret != 0)
            {
                MRPC_LOG_DEBUG("Discard request, stub id {}, data length {}", context->stub_id, context->request.length());
                continue;
            }

            auto it = client->id2conn_.find(context->stub_id);
            if (it != client->id2conn_.end())
            {
                conn = it->second.get();
            }
            if (conn == nullptr)
            {
                MRPC_LOG_DEBUG("Discard request, stub id {}, data length {}", context->stub_id, context->request.length());
                continue;
            }

            conn->send_buffer.push_back(std::move(context->request));
        }
        else
        {
            conn->Write(context->request);
        }
    }
}

void NetworkClientImpl::Start()
{
    running_ = true;
    while (running_)
    {
        uv_run(&loop_, UV_RUN_DEFAULT);
    }
}

void NetworkClientImpl::Stop()
{
    running_ = false;
    uv_stop(&loop_);
}

void NetworkClientImpl::SendRequest(const std::shared_ptr<ServiceStubContext>& context)
{
    queue_.push(context);
    int ret = uv_async_send(&async_);
    assert(ret == 0);
}

NetworkClient::NetworkClient() : impl_(new NetworkClientImpl())
{
}

NetworkClient::~NetworkClient()
{
    delete impl_;
}

void NetworkClient::Start()
{
    impl_->Start();
}

void NetworkClient::Stop()
{
    impl_->Stop();
}

void NetworkClient::SendRequest(const std::shared_ptr<ServiceStubContext>& context)
{
    impl_->SendRequest(context);
}

}
