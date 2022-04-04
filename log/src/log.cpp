#include "log.h"

#include <stdarg.h>
#include <cstdio>
#include <time.h>
#include <unordered_map>
#include <functional>

namespace why {

const char* LogLevel::ToString(LogLevel::Level level) {
    switch (level) {
#define CASE(name) \
        case LogLevel::name : { \
            return #name; \
        }

        CASE(DEBUG);
        CASE(INFO);
        CASE(WARN);
        CASE(ERROR);
        CASE(FATAL);
#undef CASE
        default : {
            return "UNKNOWN";
        }
    }
}

LogLevel::Level LogLevel::FromString(const std::string &level) {
    std::string str = level;
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
#define ISLEVEL(name) \
    if (level == #name) { \
        return LogLevel::name; \
    }

    ISLEVEL(DEBUG);
    ISLEVEL(INFO);
    ISLEVEL(DEBUG);
    ISLEVEL(DEBUG);
    ISLEVEL(DEBUG);

#undef ISLEVEL
    return LogLevel::UNKNOWN;
}

LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level,
            const char* file, int32_t line, uint32_t elapse,
            uint32_t thread_id, uint32_t fiber_id, uint64_t time,
            const std::string& thread_name) :
    m_logger(logger), m_level(level), m_file(file), m_line(line),
    m_elapse(elapse), m_threadId(thread_id), m_fiberId(fiber_id),
    m_timestamp(time), m_threadName(thread_name) {

    }

void LogEvent::Format(const char* fmt, ...) {
    char* buf = nullptr;
    
    va_list al;
    va_start(al, fmt);
    int len = vasprintf(&buf, fmt, al);
    if(len != -1) {
        m_ss << std::string(buf, len);
        // 这个在 heap 缓冲区需要自己手动释放
        free(buf);
    }
    va_end(al);
}

/**
 * @description: 代表用户实际输出内容
 */
class MessageFormatItem : public LogFormatter::FormatItem {
public:
    MessageFormatItem(const std::string& str = "") {}
    void Format(LogEvent::ptr &event, std::ostream &os) override {
        os << event->GetContent();
    }
};

class LevelFormatItem : public LogFormatter::FormatItem {
public:
    LevelFormatItem(const std::string& str = "") {}
    void Format(LogEvent::ptr &event, std::ostream &os) override {
        os << LogLevel::ToString(event->GetLogLevel());
    }
};

class ElapseFormatItem : public LogFormatter::FormatItem {
public:
    ElapseFormatItem(const std::string& str = "") {}
    void Format(LogEvent::ptr &event, std::ostream &os) override {
        os << event->GetElapse();
    }
};

class NameFormatItem : public LogFormatter::FormatItem {
public:
    NameFormatItem(const std::string& str = "") {}
    void Format(LogEvent::ptr &event, std::ostream &os) override {
        os << event->GetLogger()->GetName();
    }
};

class ThreadIdFormatItem : public LogFormatter::FormatItem {
public:
    ThreadIdFormatItem(const std::string& str = "") {}
    void Format(LogEvent::ptr &event, std::ostream &os) override {
        os << event->GetThreadId();
    }
};

class FiberIdFormatItem : public LogFormatter::FormatItem {
public:
    FiberIdFormatItem(const std::string& str = "") {}
    void Format(LogEvent::ptr &event, std::ostream &os) override {
        os << event->GetFiberId();
    }
};

class ThreadNameFormatItem : public LogFormatter::FormatItem {
public:
    ThreadNameFormatItem(const std::string& str = "") {}
    void Format(LogEvent::ptr &event, std::ostream &os) override {
        os << event->GetThreadName();
    }
};

class DateTimeFormatItem : public LogFormatter::FormatItem {
public:
    DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H:%M:%S")
        :m_format(format) {
        if(m_format.empty()) {
            m_format = "%Y-%m-%d %H:%M:%S";
        }
    }

    void Format(LogEvent::ptr &event, std::ostream &os) override {
        // 将 TimeStamp 转换为格式化时间
        struct tm tm;
        time_t time = event->GetTimeStamp();
        localtime_r(&time, &tm);
        char buf[64];
        strftime(buf, sizeof(buf), m_format.c_str(), &tm);
        os << buf;
    }
private:
    std::string m_format;
};

class FilenameFormatItem : public LogFormatter::FormatItem {
public:
    FilenameFormatItem(const std::string& str = "") {}
    void Format(LogEvent::ptr &event, std::ostream &os) override {
        os << event->GetFileName();
    }
};

class LineFormatItem : public LogFormatter::FormatItem {
public:
    LineFormatItem(const std::string& str = "") {}
    void Format(LogEvent::ptr &event, std::ostream &os) override {
        os << event->GetLine();
    }
};

class NewLineFormatItem : public LogFormatter::FormatItem {
public:
    NewLineFormatItem(const std::string& str = "") {}
    void Format(LogEvent::ptr &event, std::ostream &os) override {
        os << std::endl;
    }
};


class StringFormatItem : public LogFormatter::FormatItem {
public:
    StringFormatItem(const std::string& str)
        :m_string(str) {}
    void Format(LogEvent::ptr &event, std::ostream &os) override {
        os << m_string;
    }
private:
    std::string m_string;
};

class TabFormatItem : public LogFormatter::FormatItem {
public:
    TabFormatItem(const std::string& str = "") {}
    void Format(LogEvent::ptr &event, std::ostream &os) override {
        os << "\t";
    }
private:
    std::string m_string;
};

LogFormatter::LogFormatter(const std::string &pattern) : m_pattern(pattern) {
    Parse();
}

void LogFormatter::Parse() {

}

void LogFormatter::Format(LogEvent::ptr &event,  std::string &str) {
    std::stringstream ss;
    Format(event, ss);
    str = ss.str();
}

void LogFormatter::Format(LogEvent::ptr &event,  std::ostream &ofs) {
    for (auto &item : m_items) {
        item->Format(event, ofs);
    }
}

void LogAppender::SetFormatter(LogFormatter::ptr &val) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!val) {
        m_formatter = val;
        m_hasFormatter = true;
    }
}

LogFormatter::ptr LogAppender::GetFormatter() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_formatter;
}

void StdOutLogAppender::Log(LogEvent::ptr &event, LogLevel::Level level) {
    if (m_level >= level) {
        std::lock_guard<std::mutex> lock(m_mutex);
#ifdef ENABLE_C_STYLE_PRINT
        
#else
        m_formatter->Format(std::cout, event, level);
#endif
    }
}

};