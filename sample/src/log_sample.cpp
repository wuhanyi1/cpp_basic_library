#include "log.h"
using namespace why;

int main() {
    Logger::ptr logger = std::make_shared<Logger>();
    LogAppender::ptr appender = std::make_shared<StdOutLogAppender>();
    logger->AddAppender(appender);
    LogEvent::ptr event = std::make_shared<LogEvent>(logger, LogLevel::INFO, __FILE__, __LINE__, 0, 0, 0, 0, "TEST");
    event->Format("this is %s test log", "wuhanyi");

    logger->Log(event, LogLevel::INFO);

    return 0;
}