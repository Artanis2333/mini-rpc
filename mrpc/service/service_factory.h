#pragma once

#include <cassert>
#include <memory>
#include <string>
#include <map>

#include <mrpc/service/service.h>
#include <mrpc/util/noncopyable.h>

namespace mrpc
{

class ServiceCreater
{
public:
    ServiceCreater() = default;
    virtual ~ServiceCreater() = default;

    virtual Service* NewService() const = 0;
};

template<typename T>
class ServiceCreaterImpl : public ServiceCreater
{
public:
    Service* NewService() const override { return new T; }
};

class ServiceFactory final : private NonCopyable
{
public:
    template<typename T>
    void RegisterServiceCreater(const std::string& name);

    Service* NewService(const std::string& name) const;

private:
    std::map<std::string, std::unique_ptr<ServiceCreater>> name2creater_;
};

template<typename T>
void ServiceFactory::RegisterServiceCreater(const std::string& name)
{
    assert(name2creater_.find(name) == name2creater_.end());
    name2creater_.emplace(name, new ServiceCreaterImpl<T>());
}

}
