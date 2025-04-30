#include <atomic>

#include <mrpc/error_code.mrpc.h>
#include <mrpc/service/context.h>
#include <mrpc/service/service_stub.h>
#include <mrpc/service/global_proxy.h>

namespace mrpc
{

static std::atomic<uint64_t> g_next_seq_id = 0;

int32_t ServiceStub::CallMethod(uint32_t method_code, const std::string& req_data, std::string& rsp_data)
{
    std::shared_ptr<ServiceStubContext> context = std::make_shared<ServiceStubContext>();
    InitContext(context);
    protocol_.pack(method_code, req_data, context);

    std::chrono::milliseconds timeout(endpoint_.timeout);
    context->notifier = std::unique_ptr<ServiceStubContextNotifier>(new ServiceStubContextNotifier());
    {
        // WARNING: Pay attention to the calling sequence!!!
        std::unique_lock<std::mutex> lock(context->notifier->mtx);
        GlobalProxy::SendRequest(context);
        if (context->notifier->cv.wait_for(lock, timeout) == std::cv_status::timeout)
        {
            return ERROR_TIMEOUT;
        }
    }

    if (context->ret != 0)
    {
        return context->ret;
    }
    rsp_data = std::move(context->response_payload);
    return 0;
}

void ServiceStub::CallMethod(uint32_t method_code, const std::string& req_data, const CallbackPtr& cb)
{
    std::shared_ptr<ServiceStubContext> context = std::make_shared<ServiceStubContext>();
    InitContext(context);
    if (!cb)
    {
        context->param.need_response = false;
    }
    protocol_.pack(method_code, req_data, context);

    if (cb)
    {
        context->queue = GetCurrentThreadContextPtrQueue();
        context->callback = cb;
    }
    GlobalProxy::SendRequest(context);
}

void ServiceStub::InitContext(const std::shared_ptr<ServiceStubContext>& context)
{
    context->stub_id = stub_id_;
    context->seq_id = ++g_next_seq_id;
    context->endpoint = endpoint_;
    context->protocol = protocol_;
    //TODO param;
}

}
