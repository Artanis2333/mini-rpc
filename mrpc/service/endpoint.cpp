#include <mrpc/service/endpoint.h>
#include <mrpc/service/application_config.mrpc.h>

namespace mrpc
{

Endpoint Endpoint::ParseFromConfig(const NetworkConfig& config)
{
    Endpoint endpoint;
    endpoint.protocol = config.protocol;
    endpoint.host = config.host;
    endpoint.port = config.port;
    endpoint.timeout = config.timeout;
    return endpoint;
}

}
