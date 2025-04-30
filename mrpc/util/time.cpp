#include <chrono>

#include <mrpc/util/time.h>

namespace mrpc
{

uint64_t Time::Now()
{
    auto now = std::chrono::system_clock::now();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return milliseconds.count();
}

}
