#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <variant>

#include <mrpc/service/callback.h>
#include <mrpc/service/endpoint.h>
#include <mrpc/service/protocol.h>
#include <mrpc/util/thread_safe_queue.h>

namespace mrpc
{

struct ServiceContextRequestParam
{
    bool has_thread_hash_code = false;
    uint32_t thread_hash_code = 0;
    bool need_response = true;
};

struct ServiceContext final : public std::enable_shared_from_this<ServiceContext>
{
    // Set by IO thread
    uint64_t conn_id = 0;
    uint64_t seq_id = 0;
    std::string host;
    uint32_t port = 0;
    Protocol protocol;

    ServiceContextRequestParam param;
    std::string request_payload;

    // Set by worker thread
    bool send_response = true;
    std::string response;

    bool close_connection = false;
};

struct ServiceStubContextNotifier
{
    std::mutex mtx;
    std::condition_variable cv;
};

struct ServiceStubContext final : public std::enable_shared_from_this<ServiceStubContext>
{
    // Set by worker thread
    uint64_t stub_id = 0;
    uint64_t seq_id = 0;
    Endpoint endpoint;
    Protocol protocol;

    ServiceContextRequestParam param;
    std::string request;

    std::unique_ptr<ServiceStubContextNotifier> notifier; // Sync call
    void* queue = nullptr; // Async call (thread_local ContextPtrQueue*)
    std::shared_ptr<Callback> callback;

    // Set by IO thread
    int32_t ret = 0;
    std::string response_payload;
};

using ContextPtrWrapper = std::variant<std::shared_ptr<ServiceContext>, std::shared_ptr<ServiceStubContext>>;
using ContextPtrQueue = ThreadSafeQueue<ContextPtrWrapper>;

ContextPtrQueue* GetCurrentThreadContextPtrQueue();
void SetCurrentThreadContextPtrQueue(ContextPtrQueue* queue);

}
