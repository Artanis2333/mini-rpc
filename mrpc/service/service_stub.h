#pragma once

#include <cstdint>
#include <string>
#include <memory>

#include <mrpc/service/callback.h>
#include <mrpc/service/context.h>
#include <mrpc/service/endpoint.h>
#include <mrpc/service/protocol.h>
#include <mrpc/util/noncopyable.h>

namespace mrpc
{

class GlobalProxy;

class ServiceStub : public std::enable_shared_from_this<ServiceStub>, private NonCopyable
{
public:
    ServiceStub() = default;
    virtual ~ServiceStub() = default;

    void SetProtocol(const Protocol& protocol) { protocol_ = protocol; }

    int32_t CallMethod(uint32_t method_code, const std::string& req_data, std::string& rsp_data);
    void CallMethod(uint32_t method_code, const std::string& req_data, const CallbackPtr& cb);

private:
    void InitContext(const std::shared_ptr<ServiceStubContext>& context);

    uint64_t stub_id_ = 0;
    Endpoint endpoint_;
    Protocol protocol_;
    ServiceContextRequestParam param_;

    friend class GlobalProxy;
};

}
