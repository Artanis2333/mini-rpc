#pragma once

#include <string>
#include <memory>
#include <vector>

#include <mrpc/service/application_config.mrpc.h>
#include <mrpc/service/context.h>
#include <mrpc/service/service_bridge.h>
#include <mrpc/service/service_factory.h>
#include <mrpc/util/noncopyable.h>

namespace mrpc
{

class Application final : private NonCopyable
{
public:
    using LoopCallback = void (*)(uint64_t now);

    Application() = default;
    ~Application() = default;

    int ParseArgs(int argc, char* argv[]);

    template<typename T>
    void RegisterService(const std::string& name);

    void RegisterLoopCallback(LoopCallback cb) { loop_cb_ = cb; }

    int Initialize();
    void MainLoop();
    void Finalize();

private:
    ApplicationConfig config_;
    std::vector<std::unique_ptr<ServiceBridge>> bridge_;
    ServiceFactory factory_;

    // main thread
    std::atomic<bool> running_ = false;
    LoopCallback loop_cb_ = nullptr;
    ContextPtrQueue main_queue_;
};

template<typename T>
void Application::RegisterService(const std::string& name)
{
    factory_.RegisterServiceCreater<T>(name);
}

}
