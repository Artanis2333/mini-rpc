#include <cstring>
#include <chrono>
#include <format>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>

#include <mrpc/util/log.h>

namespace mrpc
{

std::string Log::log_path_;
std::string Log::log_file_prefix_;
LogType Log::log_type_ = Roll;
LogLevel Log::mrpc_log_level_ = INFO;
LogLevel Log::log_level_ = DEBUG;

static std::mutex g_mtx;
static std::ofstream g_log_file; //TODO Roll/Day/Hour log

static void EnsureDirectoryExists(const std::string& path)
{
    try {
        if (!std::filesystem::exists(path))
        {
            std::filesystem::create_directories(path);
        }
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        std::cerr << std::format("Make Directory {} error: {}.\n", path, e.what());
        exit(1);
    }
    catch (...)
    {
        std::cerr << "Unknown error." << std::endl;
        exit(1);
    }
}

void Log::Initialize(const std::string& log_path, std::string& log_file_prefix, LogType type)
{
    log_path_ = log_path;
    log_file_prefix_ = log_file_prefix;
    log_type_ = type;

    std::string file_full_path = log_path_;
    if (log_path_.back() != '/')
    {
        file_full_path.push_back('/');
    }
    file_full_path.append(log_file_prefix_);
    file_full_path.append(".log");

    EnsureDirectoryExists(log_path_);
    g_log_file = std::ofstream(file_full_path, std::ios::out | std::ios::app);
    if (!g_log_file)
    {
        std::cerr << std::format("Cannot open file {}.\n", file_full_path);
        exit(1);
    }
}

void Log::Finalize()
{
    g_log_file.close();
}

void Log::Write(LogLevel level, std::string_view fmt)
{
    auto now = std::chrono::system_clock::now();
    auto now_s = std::chrono::zoned_time(std::chrono::current_zone(), std::chrono::time_point_cast<std::chrono::seconds>(now));
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::lock_guard<std::mutex> lock(g_mtx); //TODO Write log in another thread.
    g_log_file << std::format("{:%Y-%m-%d %H:%M:%S}.{:03d}|{}|", now_s, now_ms.count(), LogLevel_Name(level));
    g_log_file << fmt << std::endl;
}

const char* Log::GetFileNameFromPath(const char* name)
{
    const char* p = strrchr(name, '/');
    if (p != nullptr) return p + 1;
    return name;
}

}
