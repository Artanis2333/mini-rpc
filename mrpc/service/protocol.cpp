#include <cassert>
#include <cstring>
#include <mutex>
#include <map>

#include <mrpc/error_code.mrpc.h>
#include <mrpc/message/message_internal.h>
#include <mrpc/service/context.h>
#include <mrpc/service/protocol.h>
#include <mrpc/service/protocol.mrpc.h>
#include <mrpc/service/service.h>
#include <mrpc/util/log.h>

namespace mrpc
{

#define MRPC_PROTOCOL_MAGIC "MRPC"

#ifdef _MSC_VER
#pragma pack(push, 1)
struct MrpcProtocolLayout
#else
struct __attribute__((packed)) MrpcProtocolLayout
#endif
{
    char magic[4]; //"MRPC"
    uint32_t length;
    uint64_t seq_id;
    char payload[0];
};
#ifdef _MSC_VER
#pragma pack(pop)
#endif

static inline void MrpcProtocolPackHeader(uint64_t seq_id, const std::string& payload, std::string& packet)
{
    MrpcProtocolLayout layout;
    memcpy(layout.magic, MRPC_PROTOCOL_MAGIC, sizeof(layout.magic));
    layout.length = HTOLE((uint32_t)payload.length());
    layout.seq_id = HTOLE(seq_id);
    packet.assign((char*)&layout, sizeof(layout));
    packet.append(payload);
}

static inline bool MrpcProtocolUnpackHeader(const char* ptr, size_t size, bool strict, bool& has_error, uint64_t& seq_id, const char*& payload_ptr, uint32_t& payload_length)
{
    const MrpcProtocolLayout* layout = (const MrpcProtocolLayout*)ptr;
    if (size < sizeof(*layout))
    {
        return false;
    }
    if (memcmp(layout->magic, MRPC_PROTOCOL_MAGIC, sizeof(layout->magic)) != 0)
    {
        if (strict)
        {
            has_error = true;
        }
        return false;
    }

    payload_length = LETOH(layout->length);
    if (sizeof(*layout) + payload_length > size)
    {
        return false;
    }

    seq_id = LETOH(layout->seq_id);
    payload_ptr = layout->payload;
    return true;
}

static bool MrpcProtocolParse(const char*& ptr, size_t size, bool strict, bool& has_error, std::shared_ptr<ServiceContext>& context)
{
    uint64_t seq_id = 0;
    const char* payload_ptr = nullptr;
    uint32_t payload_length = 0;
    if (!MrpcProtocolUnpackHeader(ptr, size, strict, has_error, seq_id, payload_ptr, payload_length))
    {
        return false;
    }

    MrpcRequestPayload req;
    if (!req.ParseFromString(std::string_view(payload_ptr, payload_length)))
    {
        has_error = true;
        return false;
    }

    context = std::make_shared<ServiceContext>();
    context->seq_id = seq_id;
    context->param.has_thread_hash_code = req.param.has_thread_hash_code;
    context->param.thread_hash_code = req.param.thread_hash_code;
    context->param.need_response = req.param.need_response;
    context->request_payload = std::move(req.request_payload);

    ptr += sizeof(MrpcProtocolLayout) + payload_length;
    return true;
}

static void MrpcProtocolHandleRequest(Service& service, const std::shared_ptr<ServiceContext>& context)
{
    MrpcMethodRequest req;
    int32_t ret = 0;
    std::string rsp_data;
    do
    {
        if (!req.ParseFromString(context->request_payload))
        {
            ret = ERROR_INVALID_SERVICE_REQUEST_DATA;
            break;
        }

        try
        {
            ret = service.CallMethod(req.method_code, req.req_data, rsp_data);
        }
        catch (const std::exception& e)
        {
            ret = ERROR_EXCEPTION;
            MRPC_LOG_ERROR("Catch exception {}", e.what());
        }
        catch (...)
        {
            ret = ERROR_EXCEPTION;
            MRPC_LOG_ERROR("Catch unknown exception.");
        }
    } while (0);

    if (context->param.need_response && context->send_response)
    {
        MrpcMethodResponse rsp;
        rsp.ret = ret;
        rsp.rsp_data = std::move(rsp_data);

        std::string response_payload;
        rsp.SerializeToString(response_payload);
        MrpcProtocolPackHeader(context->seq_id, response_payload, context->response);
    }
}

static void MrpcProtocolPack(uint32_t method_code, const std::string& req_data, std::shared_ptr<ServiceStubContext>& context)
{
    MrpcMethodRequest req;
    req.method_code = method_code;
    req.req_data = req_data;

    MrpcRequestPayload req_payload;
    req_payload.param.has_thread_hash_code = context->param.has_thread_hash_code;
    req_payload.param.thread_hash_code = context->param.thread_hash_code;
    req_payload.param.need_response = context->param.need_response;
    req.SerializeToString(req_payload.request_payload);

    std::string request_payload;
    req_payload.SerializeToString(request_payload);

    MrpcProtocolPackHeader(context->seq_id, request_payload, context->request);
}

static bool MrpcProtocolHandleResponse(const char*& ptr, size_t size, bool& has_error, uint64_t& seq_id, int32_t& ret, std::string& response_payload)
{
    const char* payload_ptr = nullptr;
    uint32_t payload_length = 0;
    if (!MrpcProtocolUnpackHeader(ptr, size, true, has_error, seq_id, payload_ptr, payload_length))
    {
        return false;
    }

    MrpcMethodResponse rsp;
    if (!rsp.ParseFromString(std::string_view(payload_ptr, payload_length)))
    {
        has_error = true;
        return false;
    }

    ret = rsp.ret;
    response_payload = std::move(rsp.rsp_data);
    ptr += sizeof(MrpcProtocolLayout) + payload_length;
    return true;
}

static std::mutex g_mutex;
static std::map<std::string, Protocol> g_name2protocol =
{
    { "mrpc", 
        { 
            .parse = MrpcProtocolParse, 
            .handle_request = MrpcProtocolHandleRequest, 
            .pack = MrpcProtocolPack,
            .handle_response = MrpcProtocolHandleResponse,
        }
    },
};

void GlobalRegisterProtocol(const std::string& name, const Protocol& protocol)
{
    std::lock_guard<std::mutex> lock(g_mutex);
    assert(g_name2protocol.find(name) == g_name2protocol.end());
    g_name2protocol[name] = protocol;
}

const Protocol* GlobalFindProtocol(const std::string& name)
{
    std::lock_guard<std::mutex> lock(g_mutex);
    auto it = g_name2protocol.find(name);
    if (it != g_name2protocol.end())
    {
        return &it->second;
    }
    return nullptr;
}

}
