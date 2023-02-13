/*
 * @Author: wuhanyi
 * @Date: 2022-04-03 20:44:37
 * @LastEditTime: 2023-02-09 14:43:30
 * @FilePath: /cpp_basic_library/src/log/log.h
 * @Description: 日志模块
 * 
 * Copyright (c) 2022 by wuhanyi, All Rights Reserved. 
 */
#ifndef __WHY_LOG_H__
#define __WHY_LOG_H__

#include <memory>
#include <algorithm>
#include <sstream>
#include <stdint.h>
#include <mutex>
#include <iostream>
#include <vector>
#include <list>
#include <fstream>
#include <unordered_map>
#include "common.h"

/**
 * @brief 使用流式方式将 level 级别的日志写入到 logger
 */
#define WHY_LOG_LEVEL_WITH_STREAM(logger, level) \
    if (logger->GetLevel() <= level) \
        why::LogEventWrap(std::make_shared<why::LogEvent>(logger, level, __FILE__, __LINE__, \
            0, 0, 0, why::GetCurrentSec(), why::ThisThread::GetName().c_str())).GetSS()

#define WHY_LOG_DEBUG_WITH_STREAM(logger) WHY_LOG_LEVEL_WITH_STREAM(logger, why::LogLevel::DEBUG)

#define WHY_LOG_INFO_WITH_STREAM(logger) WHY_LOG_LEVEL_WITH_STREAM(logger, why::LogLevel::INFO)

#define WHY_LOG_WARN_WITH_STREAM(logger) WHY_LOG_LEVEL_WITH_STREAM(logger, why::LogLevel::WARN)

#define WHY_LOG_ERROR_WITH_STREAM(logger) WHY_LOG_LEVEL_WITH_STREAM(logger, why::LogLevel::ERROR)

#define WHY_LOG_FATAL_WITH_STREAM(logger) WHY_LOG_LEVEL_WITH_STREAM(logger, why::LogLevel::FATAL)

/**
 * @description: 以 fmt 方式向指定日志器写入日志的宏
 */
#define WHY_LOG_LEVEL(logger, level, ...)                                                                        \
    if (level >= logger->GetLevel()) {                                                                                \
        auto event = std::make_shared<why::LogEvent>(logger, level, __FILE__, __LINE__, 0, 0, 0,                   \
            why::GetCurrentSec(), why::ThisThread::GetName().c_str());                                              \
        event->Format(__VA_ARGS__);                                                                              \
        logger->Log(event, level);                                                                                    \
    }

#define WHY_LOG_DEBUG(logger, ...) WHY_LOG_LEVEL(logger, why::LogLevel::DEBUG, __VA_ARGS__)

#define WHY_LOG_INFO(logger, ...) WHY_LOG_LEVEL(logger, why::LogLevel::INFO, __VA_ARGS__)

#define WHY_LOG_WARN(logger, ...) WHY_LOG_LEVEL(logger, why::LogLevel::WARN, __VA_ARGS__)

#define WHY_LOG_ERROR(logger, ...) WHY_LOG_LEVEL(logger, why::LogLevel::ERROR, __VA_ARGS__)

#define WHY_LOG_FATAL(logger, ...) WHY_LOG_LEVEL(logger, why::LogLevel::FATAL, __VA_ARGS__)

/**
 * @description: 获取主日志器
 */
#define LOG_ROOT() why::LoggerManager::LoggerMgr::Instance().GetRoot()

/**
 * @description: 获取name的日志器
 */
#define LOG_NAME(name) why::LoggerManager::LoggerMgr::Instance().GetLogger(name)

/**
 * @description: 默认的日志宏，即直接向主日志器写入的宏
 */

#define LOG_DEBUG(...) WHY_LOG_LEVEL(LOG_ROOT(), why::LogLevel::DEBUG, __VA_ARGS__)

#define LOG_INFO(...) WHY_LOG_LEVEL(LOG_ROOT(), why::LogLevel::INFO, __VA_ARGS__)

#define LOG_WARN(...) WHY_LOG_LEVEL(LOG_ROOT(), why::LogLevel::WARN, __VA_ARGS__)

#define LOG_ERROR(...) WHY_LOG_LEVEL(LOG_ROOT(), why::LogLevel::ERROR, __VA_ARGS__)

#define LOG_FATAL(...) WHY_LOG_LEVEL(LOG_ROOT(), why::LogLevel::FATAL, __VA_ARGS__)

namespace why {

/**
 * @description: 主要记录日志级别的枚举值和两个功能性的 static 函数
 */
class LogLevel {
public:    
    /**
     * @description: 枚举，表示日志级别
     */    
    enum Level {
        UNKNOWN = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };

    /**
     * @description: 返回该日志级别(enum 类型)的字符串输出
     * @param {Level} level：传入的日志级别
     * @return {const char*}: 这里返回的是存储在 .rodata section 中的字符串地址
     */
    static const char* ToString(LogLevel::Level level);

    static LogLevel::Level FromString(const std::string& str);
};

class Logger;
class LogEvent {
public:
    using ptr = std::shared_ptr<LogEvent>;

    LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level,
            const char* file, int32_t line, uint32_t elapse,
            uint32_t thread_id, uint32_t fiber_id, uint64_t time,
            const std::string& thread_name);
    
    const char* GetFileName() const { return m_file; }

    uint32_t GetLine() const { return m_line; }

    uint32_t GetElapse() const { return m_elapse;}

    uint32_t GetThreadId() const { return m_threadId; }

    uint32_t GetFiberId() const { return m_fiberId; }

    uint64_t GetTimeStamp() const { return m_timestamp; }

    const std::string& GetThreadName() const { return m_threadName; }

    const std::string GetContent() const { return m_ss.str(); }

    std::shared_ptr<Logger> GetLogger() const { return m_logger; }

    LogLevel::Level GetLevel() const { return m_level; }

    /**
     * @description: 返回日志内容字符串流
     */
    std::stringstream& GetSS() { return m_ss; }

    /**
     * @description: 格式化写入日志内容
     */
    void Format(const char* fmt, ...);

private:
    // 日志器
    std::shared_ptr<Logger> m_logger;
    // 日志等级
    LogLevel::Level m_level{LogLevel::DEBUG};
    // 日志输出所在文件名
    const char *m_file{nullptr};
    // 行号
    uint32_t m_line{0};
    // 程序启动开始到现在的毫秒数
    uint32_t m_elapse{0};
    // 线程ID
    uint32_t m_threadId{0};
    // 协程ID
    uint32_t m_fiberId{0};
    // 时间戳
    uint64_t m_timestamp{0};
    // 线程名称
    std::string m_threadName;
    // 日志内容流
    std::stringstream m_ss;
};

class LogFormatter {
public:
    using ptr = std::shared_ptr<LogFormatter>;
    
    LogFormatter(const std::string &pattern);

    void Parse();
    
    /**
     * @description: 按照 pattern 将格式化的日志信息输出到传出参数 str 中
     * @param[out] str: 传出参数
     */
    void Format(LogEvent::ptr event, std::string &str, size_t &pos);
    
    /**
     * @description: 相同，输出到流中
     */
    void Format(LogEvent::ptr event, std::ostream &ofs);

    bool IsError() const { return m_error;}

    std::string GetPattern() const { return m_pattern; }

public:
    class FormatItem {
    public:
        using ptr = std::shared_ptr<FormatItem>;
        virtual ~FormatItem() = default;
        /**
         * @description: 将 Event 中自己这个 Item 所匹配的属性值输出到 ofs 中
         */
        virtual void Format(LogEvent::ptr event, std::ostream &os) = 0;

        /**
         * @description: 相同,不过这里用 std::string 来接收
         * @param[out] str: 传出参数,保存信息的字符串
         * @param[in] pos: 即字符串当前可以写入的位置,会在函数内部被修改
         */
        virtual void Format(LogEvent::ptr event, std::string &str, size_t &pos) = 0;
    };

private:
    std::string m_pattern;
    std::vector<FormatItem::ptr> m_items;
    bool m_error{false};
    
};

class LogAppender {
friend class Logger;    
public:
    using ptr = std::shared_ptr<LogAppender>;

    LogAppender() = default;

    virtual ~LogAppender() = default;

    virtual void Log(LogEvent::ptr event, LogLevel::Level level) = 0;

    void SetFormatter(const LogFormatter::ptr val);

    LogFormatter::ptr GetFormatter();

    LogLevel::Level GetLevel() const { return m_level; }

    // 为何 level 的访问不加锁
    void SetLogLevel(LogLevel::Level val) { m_level = val; }

    /**
     * @description: 将日志输出目标的配置转成YAML String
     */
    virtual std::string ToYamlString() = 0;
    
protected:
    LogLevel::Level m_level{LogLevel::DEBUG};
    bool m_hasFormatter{false};
    // TODO: 这里的临界区的时间较短，后面需要改成自旋锁
    std::mutex m_mutex;
    LogFormatter::ptr m_formatter{nullptr};
};

/**
 * @description: 日志类，通过这个对象可以向其中写入日志
 */
class Logger {
public:
    using ptr = std::shared_ptr<Logger>;

    Logger(const std::string &name, Logger::ptr root = nullptr);

    void Log(LogEvent::ptr event, LogLevel::Level level);

    void AddAppender(LogAppender::ptr appender);

    void DelAppender(LogAppender::ptr appender);

    void ClearAppenders();

    LogLevel::Level GetLevel() const { return m_level; }

    void SetLogLevel(LogLevel::Level val) { m_level = val;}

    const std::string& GetName() const { return m_name; }

    void SetFormatter(LogFormatter::ptr val);

    /**
     * @description: 直接设置一个 pattern 给里面 Logger 里面的 formatter
     */
    void SetFormatter(const std::string &val);

    LogFormatter::ptr GetFormatter();

    std::string ToYamlString();
private:
    static constexpr auto kKeyDefaultPattern = "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n";

    std::string m_name;
    LogLevel::Level m_level{LogLevel::DEBUG};
    // 一个 Logger 会被多个线程共享，需要加锁
    std::mutex m_mutex;
    std::list<LogAppender::ptr> m_appenders;
    LogFormatter::ptr m_formatter;
    // 主日志器
    Logger::ptr m_root{nullptr};
};

class StdOutLogAppender : public LogAppender {
public:
    using ptr = std::shared_ptr<StdOutLogAppender>;
    void Log(LogEvent::ptr event, LogLevel::Level level) override;
    std::string ToYamlString() override;
};

class FileLogAppender : public LogAppender {
public:
    using ptr = std::shared_ptr<FileLogAppender>;
    FileLogAppender(const std::string &filename);
    void Log(LogEvent::ptr event, LogLevel::Level level) override;
    std::string ToYamlString() override;

    /**
     * @description: 重新打开日志文件
     * @return {bool} 成功返回true
     */
    bool Reopen();
private:
    /// 文件路径
    std::string m_filename;
    /// 文件流
    std::ofstream m_filestream;
    /// 上次重新打开时间
    uint64_t m_lastTime = 0;
};

/**
 * @description: 日志事件包装器
 */
class LogEventWrap {
public:
    LogEventWrap(LogEvent::ptr e);

    ~LogEventWrap();

    LogEvent::ptr GetEvent() const { return m_event; }

    /**
     * @description: 获取日志内容流
     */
    std::stringstream& GetSS();
private:
    LogEvent::ptr m_event;
};

class LoggerManager : public Noncopyable {
public:
    using LoggerMgr = why::Singleton<LoggerManager>;
    friend LoggerMgr;

    /**
     * @description: 获取指定名称的 Logger,没有就创建一个对应名字的 Logger
     */
    Logger::ptr GetLogger(const std::string &name);

    Logger::ptr GetRoot() const { return m_root; }

    void DelLogger(const std::string &name);

    /**
     * @description: 将所有的日志器配置转成YAML String
     */
    std::string ToYamlString();

private:
    LoggerManager();

private:
    static constexpr auto kKeyRootLoggerName = "root";
    std::mutex m_mutex;
    // 日志器容器
    std::unordered_map<std::string, Logger::ptr> m_loggers;
    // 主日志器
    Logger::ptr m_root;
};

}




#endif