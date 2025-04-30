#pragma once

#include <cstdint>
#include <format>
#include <source_location>
#include <string_view>
#include <utility>

namespace mrpc
{

enum LogLevel : int32_t
{
    LOGLEVEL_TRACE      = 0,
    LOGLEVEL_DEBUG      = 1,
    LOGLEVEL_INFO       = 2,
    LOGLEVEL_WARNING    = 3,
    LOGLEVEL_ERROR      = 4,
    LOGLEVEL_CRITICAL   = 5,
    LOGLEVEL_FATAL      = 6,
    LOGLEVEL_MAX        = 7,
};

/* TODO log file type
enum LogType : int32_t
{
    LOGTYPE_ROLL        = 1,
    LOGTYPE_DAY         = 2,
    LOGTYPE_HOUR        = 3,
};
*/

class Log
{
public:
    static void Init();
    static void Fini();

    static void SetLogLevel(LogLevel level);
    static bool IsLevelEnabled(LogLevel level);

    template<typename... Args>
    static inline void Write(LogLevel level, const std::source_location& location, std::format_string<Args...> fmt, Args&&... args)
    {
        Write(level, location, std::format(fmt, std::forward<Args>(args)...));
    }

    /*
    template<typename... Args>
    static void Write(LogLevel level, const std::source_location& location, std::string_view fmt, Args&&... args)
    {
        Write(level, location, std::vformat(fmt, std::make_format_args(args...)));
    }
    */

    static void Write(LogLevel level, const std::source_location& location, std::string_view fmt);
};

#define LOG_BASE(level, fmt, ...)                                           \
    do                                                                      \
    {                                                                       \
        if (!mrpc::Log::IsLevelEnabled(level))                              \
        {                                                                   \
            break;                                                          \
        }                                                                   \
        mrpc::Log::Write(level, std::source_location::current(), fmt __VA_OPT__(,) __VA_ARGS__); \
    } while (0)

#define MRPC_LOG_TRACE(fmt, ...) LOG_BASE(mrpc::LOGLEVEL_TRACE, fmt __VA_OPT__(,) __VA_ARGS__)
#define MRPC_LOG_DEBUG(fmt, ...) LOG_BASE(mrpc::LOGLEVEL_DEBUG, fmt __VA_OPT__(,) __VA_ARGS__)
#define MRPC_LOG_INFO(fmt, ...)  LOG_BASE(mrpc::LOGLEVEL_INFO, fmt __VA_OPT__(,) __VA_ARGS__)
#define MRPC_LOG_WARN(fmt, ...)  LOG_BASE(mrpc::LOGLEVEL_WARNING, fmt __VA_OPT__(,) __VA_ARGS__)
#define MRPC_LOG_ERROR(fmt, ...) LOG_BASE(mrpc::LOGLEVEL_ERROR, fmt __VA_OPT__(,) __VA_ARGS__)
#define MRPC_LOG_CRIT(fmt, ...)  LOG_BASE(mrpc::LOGLEVEL_CRITICAL, fmt __VA_OPT__(,) __VA_ARGS__)
#define MRPC_LOG_FATAL(fmt, ...) LOG_BASE(mrpc::LOGLEVEL_FATAL, fmt __VA_OPT__(,) __VA_ARGS__)

#define LOG_TRACE(fmt, ...) LOG_BASE(mrpc::LOGLEVEL_TRACE, fmt __VA_OPT__(,) __VA_ARGS__)
#define LOG_DEBUG(fmt, ...) LOG_BASE(mrpc::LOGLEVEL_DEBUG, fmt __VA_OPT__(,) __VA_ARGS__)
#define LOG_INFO(fmt, ...)  LOG_BASE(mrpc::LOGLEVEL_INFO, fmt __VA_OPT__(,) __VA_ARGS__)
#define LOG_WARN(fmt, ...)  LOG_BASE(mrpc::LOGLEVEL_WARNING, fmt __VA_OPT__(,) __VA_ARGS__)
#define LOG_ERROR(fmt, ...) LOG_BASE(mrpc::LOGLEVEL_ERROR, fmt __VA_OPT__(,) __VA_ARGS__)
#define LOG_CRIT(fmt, ...)  LOG_BASE(mrpc::LOGLEVEL_CRITICAL, fmt __VA_OPT__(,) __VA_ARGS__)
#define LOG_FATAL(fmt, ...) LOG_BASE(mrpc::LOGLEVEL_FATAL, fmt __VA_OPT__(,) __VA_ARGS__)

#define __TRY__                                                             \
    try                                                                     \
    {

#define __CATCH__                                                           \
    }                                                                       \
    catch (std::exception &e)                                               \
    {                                                                       \
        LOG_EXCEPTION(e.what());                                            \
    }                                                                       \
    catch (...)                                                             \
    {                                                                       \
        LOG_EXCEPTION("unknown exception");                                 \
    }

}
