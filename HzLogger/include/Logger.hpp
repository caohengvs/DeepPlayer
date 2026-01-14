/**
 * @file: Logger.hpp
 * @brief: 基于spdlog封装的日志记录器
 * @author: Curtis Cao
 * @date: 2025-07-16 14:07:09
 * @version: 1.0
 * @email: caohengvs888@gmail.com
 */
#pragma once
#include <memory>
#include <mutex>
#include <ostream>
#include <string>
#include <vector>
#include "ExportDef.h"

namespace spdlog
{
namespace sinks
{
class sink;
}
using sink_ptr = std::shared_ptr<sinks::sink>;
class logger;
namespace level
{
enum level_enum : int;
}
}  // namespace spdlog

#pragma warning(push)
#pragma warning(disable : 4251)  // Disable C4251 for the following code block
struct LoggerImpl;

class HZ_LIB_API Logger
{
public:
    enum LogLevel
    {
        TRACE_L = 0,
        DEBUG_L,
        INFO_L,
        WARN_L,
        ERROR_L,
        CRITICAL_L,
        OFF_L
    };

    static Logger& s_GetInstance();
    static void s_DeleteInstance();

    void Init(const std::string& loggerName = "console", LogLevel level = INFO_L, bool enableConsole = true,
              bool isSync = false, const std::string& filePath = "", size_t maxFileSize = 1048576 * 5,
              size_t maxFiles = 3);

    class LogStream;

    LogStream Log(LogLevel eLevel, const char* file, int line, const char* func);
    LogStream Trace(const char* file, int line, const char* func);
    LogStream Debug(const char* file, int line, const char* func);
    LogStream Info(const char* file, int line, const char* func);
    LogStream Warn(const char* file, int line, const char* func);
    LogStream Error(const char* file, int line, const char* func);
    LogStream Critical(const char* file, int line, const char* func);

private:
    void initSync(std::vector<spdlog::sink_ptr>, const std::string&, LogLevel);
    void initAsync(std::vector<spdlog::sink_ptr>, const std::string&, LogLevel);

private:
    Logger();
    ~Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    std::unique_ptr<LoggerImpl> pimpl_;
    static inline Logger* m_pInstance = nullptr;
    static inline std::mutex m_mtxCreate;

    friend class LogStream;
};

struct LogStreamImpl;
class HZ_LIB_API Logger::LogStream
{
public:
    LogStream(LogLevel level, LoggerImpl* loggerImpl, const char* file = nullptr, int line = 0,
              const char* func = nullptr);

    ~LogStream();

    LogStream(const LogStream&) = delete;
    LogStream& operator=(const LogStream&) = delete;
    LogStream(LogStream&&) = delete;
    LogStream& operator=(LogStream&&) = delete;

    template<typename T>
    LogStream& operator<<(const T& val);

    LogStream& operator<<(std::ostream& (*manip)(std::ostream&));

    LogStream& operator<<(const char* str);

private:
    std::unique_ptr<LogStreamImpl> pimpl_;
};
#pragma warning(pop)  // Re-enable C4251

#define INIT_LOGGER(...) Logger::s_GetInstance().init(__VA_ARGS__)
#define DELETE_LOGGER(...) Logger::s_DeleteInstance()

#define LOG_TRACE Logger::s_GetInstance().trace(__FILE__, __LINE__, __FUNCTION__)
#define LOG_DEBUG Logger::s_GetInstance().debug(__FILE__, __LINE__, __FUNCTION__)
#define LOG_INFO Logger::s_GetInstance().info(__FILE__, __LINE__, __FUNCTION__)
#define LOG_WARN Logger::s_GetInstance().warn(__FILE__, __LINE__, __FUNCTION__)
#define LOG_ERROR Logger::s_GetInstance().error(__FILE__, __LINE__, __FUNCTION__)
#define LOG_CRITICAL Logger::s_GetInstance().critical(__FILE__, __LINE__, __FUNCTION__)
