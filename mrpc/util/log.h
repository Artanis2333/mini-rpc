#pragma once

#include <format>
#include <string>
#include <string_view>

#include <mrpc/util/log.mrpc.h>

namespace mrpc
{

class Log
{
public:
    static void Initialize(const std::string& log_path, std::string& log_file_prefix, LogType type);
    static void Finalize();

    static void SetMrpcLogLevel(LogLevel level) { mrpc_log_level_ = level; }
    static inline bool IsMrpcLevelEnabled(LogLevel level) { return level >= mrpc_log_level_; }

    static void SetLogLevel(LogLevel level) { log_level_ = level; }
    static inline bool IsLevelEnabled(LogLevel level) { return level >= log_level_; }

    template<typename... Args>
    static inline void Write(LogLevel level, std::format_string<Args...> fmt, Args&&... args)
    {
        Write(level, std::format(fmt, std::forward<Args>(args)...));
    }

    static void Write(LogLevel level, std::string_view fmt);
    static const char* GetFileNameFromPath(const char* name);

private:
    static std::string log_path_;
    static std::string log_file_prefix_;
    static LogType log_type_;
    static LogLevel mrpc_log_level_;
    static LogLevel log_level_;

};

#define MRPC_LOG_BASE(level, fmt, ...)                                      \
    do                                                                      \
    {                                                                       \
        if (!mrpc::Log::IsMrpcLevelEnabled(level))                          \
        {                                                                   \
            break;                                                          \
        }                                                                   \
        mrpc::Log::Write(level, "{}|" fmt, "[MRPC]" __VA_OPT__(,) __VA_ARGS__); \
    } while (0)

#define MRPC_LOG_TRACE(fmt, ...) MRPC_LOG_BASE(mrpc::TRACE, fmt __VA_OPT__(,) __VA_ARGS__)
#define MRPC_LOG_DEBUG(fmt, ...) MRPC_LOG_BASE(mrpc::DEBUG, fmt __VA_OPT__(,) __VA_ARGS__)
#define MRPC_LOG_INFO(fmt, ...)  MRPC_LOG_BASE(mrpc::INFO, fmt __VA_OPT__(,) __VA_ARGS__)
#define MRPC_LOG_WARN(fmt, ...)  MRPC_LOG_BASE(mrpc::WARN, fmt __VA_OPT__(,) __VA_ARGS__)
#define MRPC_LOG_ERROR(fmt, ...) MRPC_LOG_BASE(mrpc::ERROR, fmt __VA_OPT__(,) __VA_ARGS__)
#define MRPC_LOG_CRIT(fmt, ...)  MRPC_LOG_BASE(mrpc::CRIT, fmt __VA_OPT__(,) __VA_ARGS__)
#define MRPC_LOG_FATAL(fmt, ...) MRPC_LOG_BASE(mrpc::FATAL, fmt __VA_OPT__(,) __VA_ARGS__)

#define LOG_BASE(level, fmt, ...)                                           \
    do                                                                      \
    {                                                                       \
        if (!mrpc::Log::IsLevelEnabled(level))                              \
        {                                                                   \
            break;                                                          \
        }                                                                   \
        mrpc::Log::Write(level, "{}:{}:{}|" fmt, mrpc::Log::GetFileNameFromPath(__FILE__), __LINE__, __FUNCTION__ __VA_OPT__(,) __VA_ARGS__); \
    } while (0)

#define LOG_TRACE(fmt, ...) LOG_BASE(mrpc::TRACE, fmt __VA_OPT__(,) __VA_ARGS__)
#define LOG_DEBUG(fmt, ...) LOG_BASE(mrpc::DEBUG, fmt __VA_OPT__(,) __VA_ARGS__)
#define LOG_INFO(fmt, ...)  LOG_BASE(mrpc::INFO, fmt __VA_OPT__(,) __VA_ARGS__)
#define LOG_WARN(fmt, ...)  LOG_BASE(mrpc::WARN, fmt __VA_OPT__(,) __VA_ARGS__)
#define LOG_ERROR(fmt, ...) LOG_BASE(mrpc::ERROR, fmt __VA_OPT__(,) __VA_ARGS__)
#define LOG_CRIT(fmt, ...)  LOG_BASE(mrpc::CRIT, fmt __VA_OPT__(,) __VA_ARGS__)
#define LOG_FATAL(fmt, ...) LOG_BASE(mrpc::FATAL, fmt __VA_OPT__(,) __VA_ARGS__)

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
