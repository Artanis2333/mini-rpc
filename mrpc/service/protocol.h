#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include <mrpc/message/message.h>

namespace mrpc
{

class Service;
class ServiceContext;
class ServiceStubContext;

struct Protocol
{
    // Server, IO thread
    using Parser = bool (*)(const char*& ptr, size_t size, bool strict, bool& has_error, std::shared_ptr<ServiceContext>& context);

    // Server, worker thread
    using RequestHandler = void (*)(Service& service, const std::shared_ptr<ServiceContext>& context);

    // Client, worker thread
    using Packer = void(*)(uint32_t method_code, const std::string& req_data, std::shared_ptr<ServiceStubContext>& context);
    using Serializer = void(*)(const std::string& method_name, const Message& req, Message& rsp);

    // Client, IO thread
    using ResponseHandler = bool (*)(const char*& ptr, size_t size, bool& has_error, uint64_t& seq_id, int32_t& ret, std::string& response_payload);

    Parser parse = nullptr;
    RequestHandler handle_request = nullptr;
    Packer pack = nullptr;
    Serializer serialize = nullptr;
    ResponseHandler handle_response = nullptr;

    bool IsServer() const { return parse != nullptr && handle_request != nullptr; }
    bool IsClient() const { return (pack != nullptr || serialize != nullptr) && handle_response != nullptr; }
};

void GlobalRegisterProtocol(const std::string& name, const Protocol& protocol);
const Protocol* GlobalFindProtocol(const std::string& name);

}
