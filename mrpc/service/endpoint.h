#pragma once

#include <cstdint>
#include <string>

namespace mrpc
{

class NetworkConfig;

struct Endpoint
{
    int32_t protocol = 0;
    std::string host;
    uint32_t port = 0;
    uint32_t timeout = 0;

    static Endpoint ParseFromConfig(const NetworkConfig& config);
};

}
