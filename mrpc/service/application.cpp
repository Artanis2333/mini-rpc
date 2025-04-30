#include <unistd.h>
#include <getopt.h>
#include <exception>
#include <format>
#include <iostream>
#include <fstream>
#include <sstream>

#include <mrpc/message/json.h>
#include <mrpc/service/application.h>
#include <mrpc/service/endpoint.h>
#include <mrpc/service/protocol.h>
#include <mrpc/service/global_proxy.h>
#include <mrpc/util/time.h>

namespace mrpc
{

static void PrintUsage(const char* bin)
{
    std::cout << std::format("Usage: {} --config config_file.json [--daemon]\n", bin);
}

static void LoadConfigFile(const std::string& file_path, ApplicationConfig& config)
{
    std::ifstream file(file_path);
    if (!file.is_open())
    {
        std::cerr << std::format("Cannot open file {}.\n", file_path);
        exit(1);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    try
    {
        JsonStringToMessage(buffer.str(), config);
    }
    catch (const std::exception& e)
    {
        std::cerr << std::format("Parse config_file {} error, {}\n", file_path, e.what());
        exit(1);
    }
}

int Application::ParseArgs(int argc, char* argv[])
{
    const char* short_opts = "c:dh";
    static struct option long_opts[] =
    {
        { "config", required_argument, nullptr, 'c'},
        { "daemon", no_argument, nullptr, 'd'},
        { "help", no_argument, nullptr, 'h'},
        { nullptr, 0, nullptr, 0}
    };

    std::string config_file;
    bool daemon_mode = false;

    int opt;
    while ((opt = getopt_long(argc, argv, short_opts, long_opts, nullptr)) != -1)
    {
        switch (opt)
        {
            case 'c':
                config_file = optarg;
                break;
            case 'd':
                daemon_mode = true;
                break;
            case 'h':
                PrintUsage(argv[0]);
                exit(0);
                break;
            case '?':
                PrintUsage(argv[0]);
                exit(0);
                break;
            default:
                break;
        }
    }

    if (config_file.empty())
    {
        PrintUsage(argv[0]);
        exit(0);
    }

    LoadConfigFile(config_file, config_);
    if (daemon_mode)
    {
        daemon(1, 0);
    }

    //TODO init log
    SetCurrentThreadContextPtrQueue(&main_queue_);

    return 0;
}

int Application::Initialize()
{
    int ret = 0;

    for (const auto& stub_config : config_.proxy.stub)
    {
        Endpoint endpoint = Endpoint::ParseFromConfig(stub_config.network);
        GlobalProxy::RegisterLocalEndpoint(stub_config.name, endpoint);
    }

    for (const auto& service_config : config_.server.service)
    {
        std::unique_ptr<ServiceBridge> bridge_ptr(new ServiceBridge());
        ret = bridge_ptr->Initialize(factory_, service_config);
        if (ret != 0)
        {
            return ret;
        }

        bridge_.emplace_back(std::move(bridge_ptr));
    }

    GlobalProxy::Start();
    for (auto& bridge_ptr : bridge_)
    {
        bridge_ptr->Start();
    }
    return 0;
}

struct MainThreadVisitor
{
    void operator()(const std::shared_ptr<ServiceContext>& context);
    void operator()(const std::shared_ptr<ServiceStubContext>& context);
};

void MainThreadVisitor::operator()(const std::shared_ptr<ServiceContext>& context)
{
    (void)context;
    assert(false);
}

void MainThreadVisitor::operator()(const std::shared_ptr<ServiceStubContext>& context)
{
    context->callback->Done(context->ret, context->response_payload);
}

void Application::MainLoop()
{
    running_ = true;

    uint32_t loop_interval = config_.common.loop_interval;
    uint64_t next_loop_time = Time::Now() + loop_interval;
    MainThreadVisitor visitor;
    std::chrono::milliseconds timeout(20);
    while (running_)
    {
        if (loop_cb_ != nullptr)
        {
            uint64_t now = Time::Now();
            if (now >= next_loop_time)
            {
                loop_cb_(now);
                next_loop_time += loop_interval;
            }
        }

        ContextPtrWrapper context_wrapper;
        if (!main_queue_.wait_and_pop(context_wrapper, timeout))
        {
            continue;
        }

        std::visit(visitor, context_wrapper);
    }
}

void Application::Finalize()
{
    for (auto& bridge_ptr : bridge_)
    {
        bridge_ptr->Stop();
    }
    GlobalProxy::Stop();
}

}
