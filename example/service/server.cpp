#include <mrpc/service/application.h>
#include <mrpc/util/log.h>

#include "service_example.mrpc.h"

class EchoServiceImpl : public example::EchoService
{
    int32_t Echo(const example::EchoRequest& req, example::EchoResponse& rsp) override
    {
        rsp.data = req.data;
        LOG_DEBUG("Send msg, {}", rsp.data);
        return 0;
    }
};

class MathServiceImpl : public example::MathService
{
    int32_t Add(const example::AddRequest& req, example::AddResponse& rsp) override
    {
        rsp.sum = req.a + req.b;
        LOG_DEBUG("{} + {} = {}", req.a, req.b, rsp.sum);
        return 0;
    }
};

void Loop(uint64_t now)
{
    (void)now;
    //LOG_DEBUG("Now: {}", now);
}

int main(int argc, char* argv[])
{
    mrpc::Application app;
    int ret = app.ParseArgs(argc, argv);
    if (ret != 0)
    {
        LOG_ERROR("Parse args error, {}", ret);
        return ret;
    }

    app.RegisterService<EchoServiceImpl>("EchoService");
    app.RegisterService<MathServiceImpl>("MathService");
    app.RegisterLoopCallback(Loop);

    ret = app.Initialize();
    if (ret != 0)
    {
        LOG_ERROR("Init error, {}", ret);
        return ret;
    }

    app.MainLoop();
    app.Finalize();
    return 0;
}
