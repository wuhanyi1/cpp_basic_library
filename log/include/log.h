/*
 * @Author: wuhanyi
 * @Date: 2022-04-03 20:44:37
 * @LastEditTime: 2022-04-05 17:52:58
 * @FilePath: /basic_library/log/include/log.h
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

/**
 * @description: 日志类，通过这个对象可以向其中写入日志
 */
class Logger {
public:
    using ptr = std::shared_ptr<Logger>;

    Logger(const std::string &name = "root");

    void Log(LogEvent::ptr event, LogLevel::Level level);

    void AddAppender(LogAppender::ptr appender);

    void DelAppender(LogAppender::ptr appender);

    void ClearAppenders();

    LogLevel::Level GetLogLevel() const { return m_level; }

    void SetLogLevel(LogLevel::Level val) { m_level = val;}

    const std::string& GetName() const { return m_name; }

    void SetFormatter(LogFormatter::ptr val);

    /**
     * @description: 直接设置一个 pattern 给里面 Logger 里面的 formatter
     */
    void SetFormatter(const std::string &val);

    LogFormatter::ptr GetFormatter();
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

    std::string GetContent() const { return m_ss.str(); }

    std::shared_ptr<Logger> GetLogger() const { return m_logger; }

    LogLevel::Level GetLogLevel() const { return m_level; }

    /**
     * @description: 返回日志内容字符串流
     */
    std::stringstream& GetSS() { return m_ss; }

    /**
     * @description: 格式化写入日志内容
     */
    void Format(const char* fmt, ...);

private:
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
    // 日志器
    std::shared_ptr<Logger> m_logger;
    // 日志等级
    LogLevel::Level m_level;
};

class LogFormatter {
public:
    using ptr = std::shared_ptr<LogFormatter>;
    
    LogFormatter(const std::string &pattern);

    void Parse();
    
    /**
     * @description: 按照 pattern 将格式化的日志信息输出到传出参数 str 中
     * @param {std::string} str: 传出参数
     */
    void Format(LogEvent::ptr event, std::string &str);
    
    /**
     * @description: 相同，输出到流中
     */
    void Format(LogEvent::ptr event, std::ostream &ofs);

public:
    class FormatItem {
    public:
        using ptr = std::shared_ptr<FormatItem>;
        
        /**
         * @description: 将 Event 中自己这个 Item 所匹配的属性值输出到 ofs 中
         */
        virtual void Format(LogEvent::ptr event, std::ostream& os);
    };

private:
    std::string m_pattern;
    std::vector<FormatItem::ptr> m_items;
    bool m_error{false};
    
};

class LogAppender {
public:
    using ptr = std::shared_ptr<LogAppender>;

    LogAppender() = default;

    virtual ~LogAppender();

    virtual void Log(LogEvent::ptr event, LogLevel::Level level) = 0;

    void SetFormatter(LogFormatter::ptr val);

    LogFormatter::ptr GetFormatter();

    LogLevel::Level GetLogLevel() const { return m_level; }

    // 为何 level 的访问不加锁
    void SetLogLevel(LogLevel::Level val) { m_level = val; }
    
protected:
    LogLevel::Level m_level{LogLevel::DEBUG};
    bool m_hasFormatter{false};
    // TODO: 这里的临界区的时间较短，后面需要改成自旋锁
    std::mutex m_mutex;
    LogFormatter::ptr m_formatter{nullptr};
};

class StdOutLogAppender : public LogAppender {
public:
    using ptr = std::shared_ptr<StdOutLogAppender>;
    void Log(LogEvent::ptr event, LogLevel::Level level) override;
};

class FileLogAppender : public LogAppender {
public:
    using ptr = std::shared_ptr<FileLogAppender>;
    FileLogAppender(const std::string &filename);
    void Log(LogEvent::ptr event, LogLevel::Level level) override;

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

};




#endif