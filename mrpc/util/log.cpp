#include <cstring>
#include <chrono>
#include <mrpc/util/log.h>
#include <iostream>

static const char* kLogLevelName[] = { "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "CRIT", "FATAL" };
static_assert((sizeof(kLogLevelName) / sizeof(const char*)) == mrpc::LOGLEVEL_MAX);

static inline const char* GetLogLevelName(mrpc::LogLevel level)
{
    if (level >= 0 && level < mrpc::LOGLEVEL_MAX)
    {
        return kLogLevelName[level];
    }
    return "";
}

static inline const char* GetFileNameFromPath(const char* name)
{
    const char* p = strrchr(name, '/');
    if (p != nullptr) return p + 1;
    return name;
}

namespace mrpc
{

void Log::Init()
{
}

void Log::Fini()
{
}

void SetLogLevel(LogLevel level)
{
    (void)level;
}

bool Log::IsLevelEnabled(LogLevel level)
{
    return level >= 0;
}

void Log::Write(LogLevel level, const std::source_location& location, std::string_view fmt)
{
    auto now = std::chrono::system_clock::now();
    auto now_s = std::chrono::zoned_time(std::chrono::current_zone(), std::chrono::time_point_cast<std::chrono::seconds>(now));
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    std::cout << std::format("{:%Y-%m-%d %H:%M:%S}.{:03d}|{}:{}:{}|{}|", now_s, now_ms.count(), GetFileNameFromPath(location.file_name()), location.line(), location.function_name(), GetLogLevelName(level));
    std::cout << fmt << std::endl;
}

}
