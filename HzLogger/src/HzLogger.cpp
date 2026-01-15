#include "HzLogger.hpp"
#include <spdlog/async.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <sstream>
#include <vector>


struct LoggerImpl
{
    std::shared_ptr<spdlog::logger> logger_;

    static spdlog::level::level_enum mapLogLevel(HzLogger::LogLevel level)
    {
        switch (level)
        {
            case HzLogger::TRACE_L:
                return spdlog::level::trace;
            case HzLogger::DEBUG_L:
                return spdlog::level::debug;
            case HzLogger::INFO_L:
                return spdlog::level::info;
            case HzLogger::WARN_L:
                return spdlog::level::warn;
            case HzLogger::ERROR_L:
                return spdlog::level::err;
            case HzLogger::CRITICAL_L:
                return spdlog::level::critical;
            case HzLogger::OFF_L:
                return spdlog::level::off;
            default:
                return spdlog::level::info;
        }
    }

    LoggerImpl() = default;

    ~LoggerImpl() = default;
};

HzLogger::HzLogger()
    : pimpl_(std::make_unique<LoggerImpl>())
{
}

HzLogger::~HzLogger() = default;

HzLogger& HzLogger::s_GetInstance()
{
    if (m_pInstance)
        return *m_pInstance;

    std::lock_guard lock(m_mtxCreate);
    m_pInstance = new HzLogger;
    return *m_pInstance;
}

void HzLogger::s_DeleteInstance()
{
    if (!m_pInstance)
        return;

    std::lock_guard lock(m_mtxCreate);
    delete m_pInstance;
    m_pInstance = nullptr;
}

void HzLogger::Init(const std::string& loggerName, HzLogger::LogLevel level, bool enableConsole, bool isSync,
                  const std::string& filePath, size_t maxFileSize, size_t maxFiles)
{
    if (pimpl_->logger_)
        return;

    std::vector<spdlog::sink_ptr> sinks;
    if (enableConsole)
    {
        sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        sinks.push_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());
    }
    if (!filePath.empty())
    {
        sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(filePath, maxFileSize, maxFiles));
    }

    if (isSync)
        initSync(sinks, loggerName, level);
    else
        initAsync(sinks, loggerName, level);
}

void HzLogger::initSync(std::vector<spdlog::sink_ptr> sinks, const std::string& loggerName, LogLevel level)
{
    if (sinks.empty())
    {
        pimpl_->logger_ = spdlog::stdout_color_mt(loggerName);
        return;
    }

    if (sinks.size() == 1)
    {
        pimpl_->logger_ = std::make_shared<spdlog::logger>(loggerName, sinks[0]);
    }
    else
    {
        pimpl_->logger_ = std::make_shared<spdlog::logger>(loggerName, begin(sinks), end(sinks));
    }
    pimpl_->logger_->set_level(LoggerImpl::mapLogLevel(level));
    pimpl_->logger_->set_pattern("[%Y-%m-%d %H:%M:%S.%e][%^%l%$][TID:%t][%s:%#:%!] %v");
    spdlog::set_default_logger(pimpl_->logger_);
    pimpl_->logger_->flush_on(LoggerImpl::mapLogLevel(level));
}

void HzLogger::initAsync(std::vector<spdlog::sink_ptr> sinks, const std::string& loggerName, LogLevel level)
{
    if (sinks.empty())
    {
        pimpl_->logger_ = spdlog::stdout_color_mt(loggerName);
        return;
    }

    spdlog::init_thread_pool(8192, 1);
    if (sinks.size() == 1)
    {
        pimpl_->logger_ = std::make_shared<spdlog::async_logger>(loggerName, sinks[0], spdlog::thread_pool(),
                                                                 spdlog::async_overflow_policy::block);
    }
    else
    {
        pimpl_->logger_ = std::make_shared<spdlog::async_logger>(
            loggerName, begin(sinks), end(sinks), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
    }

    spdlog::flush_every(std::chrono::milliseconds(500));
    pimpl_->logger_->set_level(LoggerImpl::mapLogLevel(level));
    pimpl_->logger_->set_pattern("[%Y-%m-%d %H:%M:%S.%e][%^%l%$][TID:%t][%s:%#:%!] %v");
    spdlog::set_default_logger(pimpl_->logger_);
    pimpl_->logger_->flush_on(LoggerImpl::mapLogLevel(level));
}

struct LogStreamImpl
{
    HzLogger::LogLevel level_;
    std::ostringstream ss_;
    std::shared_ptr<spdlog::logger> logger_;
    const char* file_;
    int line_;
    const char* func_;

    LogStreamImpl(HzLogger::LogLevel level, std::shared_ptr<spdlog::logger> logger, const char* file, int line,
                  const char* func)
        : level_(level)
        , logger_(std::move(logger))
        , file_(file)
        , line_(line)
        , func_(func)
    {
    }
    ~LogStreamImpl() = default;
};

HzLogger::LogStream::LogStream(HzLogger::LogLevel level, LoggerImpl* loggerImpl, const char* file, int line,
                             const char* func)
    : pimpl_(std::make_unique<LogStreamImpl>(level, loggerImpl->logger_, file, line, func))
{
}

HzLogger::LogStream::~LogStream()
{
    if (pimpl_ && pimpl_->logger_)
    {
        pimpl_->logger_->log({pimpl_->file_, pimpl_->line_, pimpl_->func_}, LoggerImpl::mapLogLevel(pimpl_->level_),
                             pimpl_->ss_.str());
    }
}

template<typename T>
HzLogger::LogStream& HzLogger::LogStream::operator<<(const T& val)
{
    if (pimpl_ && pimpl_->ss_)
    {
        pimpl_->ss_ << val;
    }
    return *this;
}

HzLogger::LogStream& HzLogger::LogStream::operator<<(std::ostream& (*manip)(std::ostream&))
{
    manip(pimpl_->ss_);
    return *this;
}

HzLogger::LogStream& HzLogger::LogStream::operator<<(const char* str)
{
    if (pimpl_ && pimpl_->ss_)
    {
        pimpl_->ss_ << str;
    }
    return *this;
}

HzLogger::LogStream HzLogger::Log(LogLevel eLevel, const char* file, int line, const char* func)
{
    return LogStream(eLevel, pimpl_.get(), file, line, func);
}

HzLogger::LogStream HzLogger::Trace(const char* file, int line, const char* func)
{
    return LogStream(TRACE_L, pimpl_.get(), file, line, func);
}

HzLogger::LogStream HzLogger::Debug(const char* file, int line, const char* func)
{
    return LogStream(DEBUG_L, pimpl_.get(), file, line, func);
}

HzLogger::LogStream HzLogger::Info(const char* file, int line, const char* func)
{
    return LogStream(INFO_L, pimpl_.get(), file, line, func);
}

HzLogger::LogStream HzLogger::Warn(const char* file, int line, const char* func)
{
    return LogStream(WARN_L, pimpl_.get(), file, line, func);
}

HzLogger::LogStream HzLogger::Error(const char* file, int line, const char* func)
{
    return LogStream(ERROR_L, pimpl_.get(), file, line, func);
}

HzLogger::LogStream HzLogger::Critical(const char* file, int line, const char* func)
{
    return LogStream(CRITICAL_L, pimpl_.get(), file, line, func);
}

// 显式模板实例化
template HZ_LIB_API HzLogger::LogStream& HzLogger::LogStream::operator<<(const char&);
template HZ_LIB_API HzLogger::LogStream& HzLogger::LogStream::operator<<(const short&);
template HZ_LIB_API HzLogger::LogStream& HzLogger::LogStream::operator<<(const int&);
template HZ_LIB_API HzLogger::LogStream& HzLogger::LogStream::operator<<(const long&);
template HZ_LIB_API HzLogger::LogStream& HzLogger::LogStream::operator<<(const long long&);
template HZ_LIB_API HzLogger::LogStream& HzLogger::LogStream::operator<<(const unsigned char&);
template HZ_LIB_API HzLogger::LogStream& HzLogger::LogStream::operator<<(const unsigned short&);
template HZ_LIB_API HzLogger::LogStream& HzLogger::LogStream::operator<<(const unsigned int&);
template HZ_LIB_API HzLogger::LogStream& HzLogger::LogStream::operator<<(const unsigned long&);
template HZ_LIB_API HzLogger::LogStream& HzLogger::LogStream::operator<<(const unsigned long long&);
template HZ_LIB_API HzLogger::LogStream& HzLogger::LogStream::operator<<(const float&);
template HZ_LIB_API HzLogger::LogStream& HzLogger::LogStream::operator<<(const double&);
template HZ_LIB_API HzLogger::LogStream& HzLogger::LogStream::operator<<(const long double&);
template HZ_LIB_API HzLogger::LogStream& HzLogger::LogStream::operator<<(const bool&);
template HZ_LIB_API HzLogger::LogStream& HzLogger::LogStream::operator<<(const void* const&);
template HZ_LIB_API HzLogger::LogStream& HzLogger::LogStream::operator<<(const std::string&);

// C++17 string_view 支持
#if __cplusplus >= 201703L
#include <string_view>
template HZ_LIB_API HzLogger::LogStream& HzLogger::LogStream::operator<<(const std::string_view&);
#endif