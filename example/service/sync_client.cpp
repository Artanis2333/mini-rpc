#include <mrpc/service/application.h>
#include <mrpc/service/global_proxy.h>
#include <mrpc/util/log.h>

#include "service_example.mrpc.h"

int main(int argc, char* argv[])
{
    mrpc::Application app;
    int ret = app.ParseArgs(argc, argv);
    if (ret != 0)
    {
        LOG_ERROR("Parse args error, {}", ret);
        return ret;
    }

    ret = app.Initialize();
    if (ret != 0)
    {
        LOG_ERROR("Init error, {}", ret);
        return ret;
    }

    auto stub = mrpc::GlobalProxy::FindServiceStub<example::EchoServiceStub>("EchoService");

    const char* message[] = { "Hello!!!", "123456", "OK." };
    for (int i = 0; i < 3; ++i)
    {
        example::EchoRequest req;
        req.data = message[i];
        example::EchoResponse rsp;
        int32_t ret = stub->Echo(req, rsp);
        LOG_INFO("data {}, ret {}", rsp.data, ret);
    }

    app.MainLoop();
    app.Finalize();
    return 0;
}
