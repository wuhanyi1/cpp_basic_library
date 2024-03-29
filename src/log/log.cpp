#include "log.h"

#include <stdarg.h>
#include <cstdio>
#include <cstring>
#include <time.h>
#include <unordered_map>
#include <functional>
#include <yaml-cpp/yaml.h>
#include "config.h"

using namespace why;

namespace why {

static constexpr auto kKeyStdOutAppender = "StdOutLogAppender";
static constexpr auto kKeyFileAppender = "FileLogAppender";

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
    std::transform(level.begin(), level.end(), str.begin(), ::toupper);
#define ISLEVEL(name) \
    if (str == #name) { \
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

Logger::Logger(const std::string& name, Logger::ptr root) : 
        m_name(name), 
        m_formatter(std::make_shared<LogFormatter>(kKeyDefaultPattern)),
        m_root(root) {

}


void Logger::Log(LogEvent::ptr event, LogLevel::Level level) {
    if(level >= m_level) {
        LOCK_GUARD lock(m_mutex);
        if(!m_appenders.empty()) {
            for(auto &i : m_appenders) {
                i->Log(event, level);
            }
        } else if(m_root) {
            m_root->Log(event, level);
        }
    }
}

void Logger::AddAppender(LogAppender::ptr appender) {
    LOCK_GUARD lock(m_mutex);
    if (!appender->m_hasFormatter) {
        appender->SetFormatter(m_formatter);
    }
    m_appenders.push_back(appender);
}

void Logger::DelAppender(LogAppender::ptr appender) {
    LOCK_GUARD lock(m_mutex);
    for(auto it = m_appenders.begin();
            it != m_appenders.end(); ++it) {
        if(*it == appender) {
            m_appenders.erase(it);
            break;
        }
    }
}

void Logger::ClearAppenders() {
    LOCK_GUARD lock(m_mutex);
    m_appenders.clear();
}

void Logger::SetFormatter(const LogFormatter::ptr val) {
    LOCK_GUARD lock(m_mutex);
    if (val) {
        m_formatter = val;
        for(auto &i : m_appenders) {
            i->SetFormatter(val);
        }
    }
}

void Logger::SetFormatter(const std::string &val) {
    auto formatter = std::make_shared<LogFormatter>(val);
    if (formatter->IsError()) {
        printf("SetFormatter for Logger:%s failed, pattern:%s is invalid\n", m_name.c_str(), val.c_str());
        return;
    }
    LOCK_GUARD lock(m_mutex);
    m_formatter = formatter;
    for (auto &i : m_appenders) {
        if (!i->m_hasFormatter) {
            i->SetFormatter(formatter);
        }
    }
}

std::string Logger::ToYamlString() {
    LOCK_GUARD lock(m_mutex);
    YAML::Node node(YAML::NodeType::Map);
    node["name"] = m_name;
    if(m_level != LogLevel::UNKNOWN) {
        node["level"] = LogLevel::ToString(m_level);
    }
    if(m_formatter) {
        node["formatter"] = m_formatter->GetPattern();
    }

    for(auto& i : m_appenders) {
        node["appenders"].push_back(YAML::Load(i->ToYamlString()));
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
}

LogFormatter::ptr Logger::GetFormatter() {
    LOCK_GUARD lock(m_mutex);
    return m_formatter;
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
    void Format(LogEvent::ptr event, std::ostream &os) override {
        os << event->GetContent();
    }
    void Format(LogEvent::ptr event, std::string &str, size_t &pos) override {
        const std::string cur_str = std::move(event->GetContent());
        if (cur_str.size() + pos > str.size()) {
            str.resize(2 * str.size());
        }
        memcpy(&str[0] + pos, &cur_str[0], cur_str.size());
        pos += cur_str.size();
    }
};

class LevelFormatItem : public LogFormatter::FormatItem {
public:
    LevelFormatItem(const std::string& str = "") {}
    void Format(LogEvent::ptr event, std::ostream &os) override {
        os << LogLevel::ToString(event->GetLevel());
    }
    void Format(LogEvent::ptr event, std::string &str, size_t &pos) override {
        std::string cur_str = LogLevel::ToString(event->GetLevel());
        if (cur_str.size() + pos > str.size()) {
            str.resize(2 * str.size());
        }
        memcpy(&str[0] + pos, &cur_str[0], cur_str.size());
        pos += cur_str.size();
    }
};

class ElapseFormatItem : public LogFormatter::FormatItem {
public:
    ElapseFormatItem(const std::string& str = "") {}
    void Format(LogEvent::ptr event, std::ostream &os) override {
        os << event->GetElapse();
    }
    void Format(LogEvent::ptr event, std::string &str, size_t &pos) override {
        std::string cur_str = std::move(std::to_string(event->GetElapse()));
        if (cur_str.size() + pos > str.size()) {
            str.resize(2 * str.size());
        }
        memcpy(&str[0] + pos, &cur_str[0], cur_str.size());
        pos += cur_str.size();
    }
};

class NameFormatItem : public LogFormatter::FormatItem {
public:
    NameFormatItem(const std::string& str = "") {}
    void Format(LogEvent::ptr event, std::ostream &os) override {
        os << event->GetLogger()->GetName();
    }
    void Format(LogEvent::ptr event, std::string &str, size_t &pos) override {
        const std::string &cur_str = event->GetLogger()->GetName();
        if (cur_str.size() + pos > str.size()) {
            str.resize(2 * str.size());
        }
        memcpy(&str[0] + pos, &cur_str[0], cur_str.size());
        pos += cur_str.size();
    }
};

class ThreadIdFormatItem : public LogFormatter::FormatItem {
public:
    ThreadIdFormatItem(const std::string& str = "") {}
    void Format(LogEvent::ptr event, std::ostream &os) override {
        os << event->GetThreadId();
    }
    void Format(LogEvent::ptr event, std::string &str, size_t &pos) override {
        std::string cur_str = std::move(std::to_string(event->GetThreadId()));
        if (cur_str.size() + pos > str.size()) {
            str.resize(2 * str.size());
        }
        memcpy(&str[0] + pos, &cur_str[0], cur_str.size());
        pos += cur_str.size();
    }
};

class FiberIdFormatItem : public LogFormatter::FormatItem {
public:
    FiberIdFormatItem(const std::string& str = "") {}
    void Format(LogEvent::ptr event, std::ostream &os) override {
        os << event->GetFiberId();
    }
    void Format(LogEvent::ptr event, std::string &str, size_t &pos) override {
        std::string cur_str = std::move(std::to_string(event->GetFiberId()));
        if (cur_str.size() + pos > str.size()) {
            str.resize(2 * str.size());
        }
        memcpy(&str[0] + pos, &cur_str[0], cur_str.size());
        pos += cur_str.size();
    }
};

class ThreadNameFormatItem : public LogFormatter::FormatItem {
public:
    ThreadNameFormatItem(const std::string& str = "") {}
    void Format(LogEvent::ptr event, std::ostream &os) override {
        os << event->GetThreadName();
    }
    void Format(LogEvent::ptr event, std::string &str, size_t &pos) override {
        const std::string &cur_str = event->GetThreadName();
        if (cur_str.size() + pos > str.size()) {
            str.resize(2 * str.size());
        }
        memcpy(&str[0] + pos, &cur_str[0], cur_str.size());
        pos += cur_str.size();
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

    void Format(LogEvent::ptr event, std::ostream &os) override {
        // 将 TimeStamp 转换为格式化时间
        struct tm tm;
        time_t time = event->GetTimeStamp();
        localtime_r(&time, &tm);
        char buf[64]{};
        strftime(buf, sizeof(buf), m_format.c_str(), &tm);
        os << buf;
    }

    void Format(LogEvent::ptr event, std::string &str, size_t &pos) override {
        std::stringstream ss;
        Format(event, ss);
        std::string cur_str = std::move(ss.str());
        if (cur_str.size() + pos > str.size()) {
            str.resize(2 * str.size());
        }
        memcpy(&str[0] + pos, &cur_str[0], cur_str.size());
        pos += cur_str.size();
    }
private:
    std::string m_format;
};

class FilenameFormatItem : public LogFormatter::FormatItem {
public:
    FilenameFormatItem(const std::string& str = "") {}
    void Format(LogEvent::ptr event, std::ostream &os) override {
        os << event->GetFileName();
    }
    void Format(LogEvent::ptr event, std::string &str, size_t &pos) override {
        const char* cur_str = event->GetFileName();
        int len = strlen(cur_str);
        str += event->GetFileName();
        if (len + pos > str.size()) {
            str.resize(2 * str.size());
        }
        memcpy(&str[0] + pos, cur_str, len);
        pos += len;
    }
};

class LineFormatItem : public LogFormatter::FormatItem {
public:
    LineFormatItem(const std::string& str = "") {}
    void Format(LogEvent::ptr event, std::ostream &os) override {
        os << event->GetLine();
    }
    void Format(LogEvent::ptr event, std::string &str, size_t &pos) override {
        std::string cur_str = std::move(std::to_string(event->GetLine()));
        if (cur_str.size() + pos > str.size()) {
            str.resize(2 * str.size());
        }
        memcpy(&str[0] + pos, &cur_str[0], cur_str.size());
        pos += cur_str.size();
    }
};

class NewLineFormatItem : public LogFormatter::FormatItem {
public:
    NewLineFormatItem(const std::string& str = "") {}
    void Format(LogEvent::ptr event, std::ostream &os) override {
        os << std::endl;
    }
    void Format(LogEvent::ptr event, std::string &str, size_t &pos) override {
        if (1 + pos > str.size()) {
            str.resize(2 * str.size());
        }
        memset(&str[0] + pos, '\n', 1);
        pos += 1;
    }
};


class StringFormatItem : public LogFormatter::FormatItem {
public:
    StringFormatItem(const std::string& str)
        :m_string(str) {}
    void Format(LogEvent::ptr event, std::ostream &os) override {
        os << m_string;
    }
    void Format(LogEvent::ptr event, std::string &str, size_t &pos) override {
        str += m_string;
        if (m_string.size() + pos > str.size()) {
            str.resize(2 * str.size());
        }
        memcpy(&str[0] + pos, &m_string[0], m_string.size());
        pos += m_string.size();
    }
private:
    std::string m_string;
};

class TabFormatItem : public LogFormatter::FormatItem {
public:
    TabFormatItem(const std::string& str = "") {}
    void Format(LogEvent::ptr event, std::ostream &os) override {
        os << "\t";
    }
    void Format(LogEvent::ptr event, std::string &str, size_t &pos) override {
        if (1 + pos > str.size()) {
            str.resize(2 * str.size());
        }
        memset(&str[0] + pos, '\t', 1);
        pos += 1;
    }
private:
    std::string m_string;
};

LogFormatter::LogFormatter(const std::string &pattern) : m_pattern(pattern) {
    Parse();
}

void LogFormatter::Parse() {
    // 模式字符串 FormatItem参数字符串(补充参数) type
    std::vector<std::tuple<std::string, std::string, int>> items;
    // 记录 normal string
    std::string nstr{};
    size_t size = m_pattern.size();
    for (size_t i = 0; i < size; i++) {
        if (m_pattern[i] != '%') {
            nstr.push_back(m_pattern[i]);
            continue;
        }
        
        // 处理 %%%% 这种情况，即 normal string 是多个连续的 %,下面是防止出现 %%%d 这种形式
        if((i + 1) < m_pattern.size()) {
            if(m_pattern[i + 1] == '%') {
                nstr.append(1, '%');
                continue;
            }
        }

        // 处理模式字符（串）,一次 while 循环的目标就是处理一个模式字符串，若有参数字符串，也要处理掉
        size_t n = i + 1;
        int status = 0;
        // 对应于上面 items 的 tuple 的参数 1 与 2
        std::string format_str{};
        std::string param_str{};
        size_t param_str_begin{0};
        while (n < size) {
            // 边界,遇到一个没有参数字串的 format_str 的结尾,status 为 0 表示正在处理模式字符
            if (!status && !isalpha(m_pattern[n]) && m_pattern[n] != '{' && m_pattern[n] != '}') {
                format_str += m_pattern.substr(i + 1, n - i - 1);
                // 这个字符是模式串的结尾，之后还要被处理，所以抵消 i++
                i = n - 1;
                // 本次模式串处理结束
                break;
            }

            if (status == 0) {
                // 遇到FormatItem参数字符串
                if (m_pattern[n] == '{') {
                    format_str += m_pattern.substr(i + 1, n - i - 1);
                    status = 1;
                    param_str_begin = n + 1;
                }
            } else if (status == 1) {
                if (m_pattern[n] == '}') {
                    param_str += m_pattern.substr(param_str_begin, n - param_str_begin);
                    status = 0;
                    // } 不需要再处理了，直接退出
                    i = n;
                    // 参数字符串处理完代表本次模式串处理结束
                    break;
                }
            }
            n++;
            if (n == size) {
                if (format_str.empty()) {
                    format_str += m_pattern.substr(i + 1);
                }
                // 此时 m_pattern[n] 已经被处理过了,否则下次 i++ == n,会把这个字符当 normal string
                i = n;
            }
        }
        // 为 0，说明模式串正常处理完毕
        if (status == 0) {
            // 说明在本次模式串处理之前有 nstr,插入到 items 数组中
            if (!nstr.empty()) {
                items.push_back(std::make_tuple(nstr, "", 0));
                nstr.clear();
            }
            // 再把本次的模式串插入到 items 中
            if (!format_str.empty()) {
                items.push_back(std::make_tuple(format_str, param_str, 1));
            }
        } else {
            // 若 status 为 1，说明模式串后面的参数字符串没处理完，即格式不正确
            m_error = true;
            std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
            // 同时在 items 数组中插入一个 normal string 的项
            items.push_back(std::make_tuple("<<pattern_error>>", "", 0));
        }
    }
    // 还有 nstr 未被处理
    if (!nstr.empty()) {
        items.push_back(std::make_tuple(nstr, "", 0));
    }
    static std::unordered_map<std::string, std::function<FormatItem::ptr(const std::string&)>> maps = {
#define MAP(fmt_str, class_type) \
        {#fmt_str, [](const std::string &str) { return std::make_shared<class_type>(str); }}

        MAP(m, MessageFormatItem),           //m:消息
        MAP(p, LevelFormatItem),             //p:日志级别
        MAP(r, ElapseFormatItem),            //r:累计毫秒数
        MAP(c, NameFormatItem),              //c:日志名称
        MAP(t, ThreadIdFormatItem),          //t:线程id
        MAP(n, NewLineFormatItem),           //n:换行
        MAP(d, DateTimeFormatItem),          //d:时间
        MAP(f, FilenameFormatItem),          //f:文件名
        MAP(l, LineFormatItem),              //l:行号
        MAP(T, TabFormatItem),               //T:Tab
        MAP(F, FiberIdFormatItem),           //F:协程id
        MAP(N, ThreadNameFormatItem)        //N:线程名称
#undef MAP        
    };

    for (auto &it : items) {
        // type 为 0 表示 normal string
        if (std::get<2>(it) == 0) {
            m_items.push_back(std::make_shared<StringFormatItem>(std::get<0>(it)));
        } else {
            auto iter = maps.find(std::get<0>(it));
            if (iter != maps.end()) {
                m_items.push_back(iter->second(std::get<1>(it)));
            } else {
                // 否则模式串没在 maps 中找到，说明模式串有问题
                m_items.push_back(std::make_shared<StringFormatItem>("<<error_format %" + std::get<0>(it) + ">>"));
            }
        }
    }
}

void LogFormatter::Format(LogEvent::ptr event,  std::string &str, size_t &pos) {
    for (auto &item : m_items) {
        item->Format(event, str, pos);
    }
}

void LogFormatter::Format(LogEvent::ptr event,  std::ostream &ofs) {
    for (auto &item : m_items) {
        item->Format(event, ofs);
    }
}

void LogAppender::SetFormatter(LogFormatter::ptr val) {
    LOCK_GUARD lock(m_mutex);
    m_formatter = val;
    if (!m_hasFormatter) {
        m_hasFormatter = true;
    }
}

LogFormatter::ptr LogAppender::GetFormatter() {
    LOCK_GUARD lock(m_mutex);
    return m_formatter;
}

void StdOutLogAppender::Log(LogEvent::ptr event, LogLevel::Level level) {
    if (level >= m_level) {
        LOCK_GUARD lock(m_mutex);
        m_formatter->Format(event, std::cout);
    }
}

std::string StdOutLogAppender::ToYamlString() {
    LOCK_GUARD lock(m_mutex);
    YAML::Node node;
    node["type"] = kKeyStdOutAppender;
    if(m_level != LogLevel::UNKNOWN) {
        node["level"] = LogLevel::ToString(m_level);
    }
    if(m_hasFormatter && m_formatter) {
        node["formatter"] = m_formatter->GetPattern();
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
}

FileLogAppender::FileLogAppender(const std::string& filename)
    :m_filename(filename) {
    Reopen();
}

bool FileLogAppender::Reopen() {
    LOCK_GUARD lock(m_mutex);
    if(m_filestream) {
        m_filestream.close();
    }
    m_filestream.open(m_filename, std::ios::app);
    return m_filestream.is_open();
}

void FileLogAppender::Log(LogEvent::ptr event, LogLevel::Level level) {
    if(level >= m_level) {
        uint64_t now = event->GetTimeStamp();
        // 3s 刷新一次文件内容，这是图啥 ？
        if(now >= (m_lastTime + 3)) {
            Reopen();
            m_lastTime = now;
        }
        LOCK_GUARD lock(m_mutex);
        m_formatter->Format(event, m_filestream);
    }
}

std::string FileLogAppender::ToYamlString() {
    LOCK_GUARD lock(m_mutex);
    YAML::Node node;
    node["type"] = kKeyFileAppender;
    node["file"] = m_filename;
    if(m_level != LogLevel::UNKNOWN) {
        node["level"] = LogLevel::ToString(m_level);
    }
    if(m_hasFormatter && m_formatter) {
        node["formatter"] = m_formatter->GetPattern();
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
}

LogEventWrap::LogEventWrap(LogEvent::ptr e)
    :m_event(e) {
}

LogEventWrap::~LogEventWrap() {
    m_event->GetLogger()->Log(m_event, m_event->GetLevel());
}

std::stringstream& LogEventWrap::GetSS() {
    return m_event->GetSS();
}

LoggerManager::LoggerManager() : m_root(std::make_shared<Logger>(kKeyRootLoggerName)) {
    m_root->AddAppender(std::make_shared<StdOutLogAppender>());
    m_loggers[kKeyRootLoggerName] = m_root;
}

Logger::ptr LoggerManager::GetLogger(const std::string &name) {
    LOCK_GUARD lock(m_mutex);
    auto iter = m_loggers.find(name);
    if (iter != m_loggers.end()) {
        return iter->second;
    }

    Logger::ptr logger = std::make_shared<Logger>(name, m_root);
    m_loggers[name] = logger;
    return logger;
}

void LoggerManager::DelLogger(const std::string& name) {
    LOCK_GUARD lock(m_mutex);
    m_loggers.erase(name);
}

std::string LoggerManager::ToYamlString() {
    LOCK_GUARD lock(m_mutex);
    YAML::Node node;
    for(auto& i : m_loggers) {
        node.push_back(YAML::Load(i.second->ToYamlString()));
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
}

struct LogAppenderConfig {
    LogAppenderConfig(int type_, LogLevel::Level level_, const std::string& formatter_, const std::string& file_) :
        type(type_), level(level_), formatter(formatter_), file(file_) {}

    int type{0};
    LogLevel::Level level{LogLevel::UNKNOWN};
    std::string formatter;
    std::string file;

    bool operator==(const LogAppenderConfig& val) const {
        return type == val.type && level == val.level &&
               formatter == val.formatter && file == val.file;
    }
};

struct LoggerConfig {
    std::string name;
    LogLevel::Level level{LogLevel::DEBUG};
    std::string formatter;
    std::vector<LogAppenderConfig> appenders;

    bool operator==(const LoggerConfig& val) const {
        return name == val.name &&
               level == val.level &&
               formatter == val.formatter &&
               appenders == val.appenders; 
    }

    bool operator<(const LoggerConfig& val) const {
        return name < val.name;
    }

};

template<>
struct LexicalCast<std::string, LoggerConfig> {
    LoggerConfig operator()(const std::string& val) {
        YAML::Node node = YAML::Load(val);
        LoggerConfig res{};
        if (!node["name"].IsDefined()) {
            CHECK_THROW(false, "Logger name is NULL");
        }
        if (!node["level"].IsDefined()) {
            CHECK_THROW(false, "Logger level is NULL");
        }
        res.name = node["name"].as<std::string>();
        res.level = LogLevel::FromString(node["level"].Scalar());
        if (node["formatter"].IsDefined()) {
            res.formatter = node["formatter"].Scalar();
        }
        if (node["appenders"].IsDefined()) {
            std::vector<LogAppenderConfig> tmp{};
            YAML::Node sub_node = node["appenders"];
            for (size_t i = 0; i < sub_node.size(); ++i) {
                LogLevel::Level level = sub_node[i]["level"].IsDefined() ?
                                            LogLevel::FromString(sub_node[i]["level"].Scalar()) :
                                            LogLevel::UNKNOWN; 
                std::string formatter = sub_node[i]["formatter"].IsDefined() ? 
                                            sub_node[i]["formatter"].Scalar() : "";
                if (!sub_node[i]["type"].IsDefined()) {
                    CHECK_THROW(false, "appender's type is NULL");
                }
                std::string type = sub_node[i]["type"].Scalar();
                if (type == kKeyFileAppender) {
                    if (!sub_node[i]["file"].IsDefined()) {
                        CHECK_THROW(false, "lack of FileAppender's Filename");
                    }
                    res.appenders.emplace_back(1, level, formatter, sub_node[i]["file"].Scalar());
                } else if (type == kKeyStdOutAppender) {
                    res.appenders.emplace_back(0, level, formatter, "");
                } else {
                    CHECK_THROW(false, "appender type error, it's [%s]", type.c_str());
                }
            }
        }
        return res;
    }
};

template<>
struct LexicalCast<LoggerConfig, std::string> {
    std::string operator()(const LoggerConfig& config) {
        YAML::Node node(YAML::NodeType::Map);
        node["name"] = config.name;
        node["level"] = LogLevel::ToString(config.level);
        if (config.appenders.empty()) {
            return (std::stringstream() << node).str();
        }
        YAML::Node arr(YAML::NodeType::Sequence);
        for (auto &i : config.appenders) {
            YAML::Node appender(YAML::NodeType::Map);
            appender["type"] = (i.type == 0) ? kKeyStdOutAppender : kKeyFileAppender;
            if (i.level != LogLevel::UNKNOWN) {
                appender["level"] = LogLevel::ToString(i.level);
            }
            if (!i.formatter.empty()) {
                appender["formatter"] = i.formatter;
            }
            if (!i.file.empty()) {
                appender["file"] = i.file;
            }
            arr.push_back(appender);
        }
        node["appenders"] = arr;
        return (std::stringstream() << node).str();
    }
};

ConfigVar<std::set<LoggerConfig>>::ptr g_logger_config_vec = 
    ConfigVarManager::LookUp("logs", std::set<LoggerConfig>(), "logs config var");

struct LogIniter {
    LogIniter() {
        g_logger_config_vec->AddListener([](const std::set<LoggerConfig>& old_val, 
                                            const std::set<LoggerConfig>& new_val) {                                                
            for (auto &i : new_val) {
                auto iter = old_val.find(i);
                // 查找 i.name 对于的 Logger，如果不存在则会创建
                auto ptr = LOG_NAME(i.name);
                // 如果是已存在的 logger
                if (iter != old_val.end()) {
                    if (*iter == i) {
                        continue;
                    } else {
                        ptr->ClearAppenders();
                    }
                }
                ptr->SetLogLevel(i.level);
                if (!i.formatter.empty() && ptr->GetFormatter()->GetPattern() != i.formatter) {
                    ptr->SetFormatter(i.formatter);
                }
                for (auto &appender : i.appenders) {
                    LogAppender::ptr cur_ap;
                    if (appender.type == 0) {
                        // root 有一个默认的 StdOutLogAppender,并且初始时不在 g_logger_config_vec 中
                        if (i.name == "root") {
                            continue;
                        }
                        cur_ap = std::make_shared<StdOutLogAppender>();
                    } else if (appender.type == 1) {
                        CHECK_THROW(!appender.file.empty(), "FileAppender's filename is empty");
                        cur_ap = std::make_shared<FileLogAppender>(appender.file);
                    } else {
                        CHECK_THROW(false, "appender.type:[%d] invalid", appender.type);
                    }
                    if (appender.level != LogLevel::UNKNOWN) {
                        cur_ap->SetLogLevel(appender.level);
                    }
                    if (!appender.formatter.empty()) {
                        LogFormatter::ptr fmt = std::make_shared<LogFormatter>(appender.formatter);
                        CHECK_THROW(fmt->IsError() != true, "formatter:[%s] is invalid", appender.formatter.c_str());
                        cur_ap->SetFormatter(fmt);
                    }
                    ptr->AddAppender(cur_ap);
                }
            }

            // delete all Logger which not in new_val from LoggerManager
            for (auto &i : old_val) {
                if (new_val.find(i) == new_val.end()) {
                    LoggerManager::LoggerMgr::Instance().DelLogger(i.name);
                }
            }
        });
    }
};

static LogIniter log_initer;

}