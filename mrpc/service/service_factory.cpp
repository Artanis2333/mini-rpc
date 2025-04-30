#include <mrpc/service/service_factory.h>

mrpc::Service* mrpc::ServiceFactory::NewService(const std::string& name) const
{
    auto it = name2creater_.find(name);
    if (it == name2creater_.end()) return nullptr;
    return it->second->NewService();
}

