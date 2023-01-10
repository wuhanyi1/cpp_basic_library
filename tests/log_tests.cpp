#include "log.h"
#include "common.h"
#include <thread>
#include <chrono>
#include <memory>
using namespace why;

auto test_logger = LOG_NAME("test");

void test_stream_with_StdOutLogAppender() {
    test_logger->ClearAppenders();
    test_logger->AddAppender(std::make_shared<StdOutLogAppender>());
    test_logger->SetLogLevel(LogLevel::WARN);

    WHY_LOG_DEBUG_WITH_STREAM(test_logger) << "WHY_LOG_DEBUG_WITH_STREAM" << " " << "test successful";
    WHY_LOG_INFO_WITH_STREAM(test_logger) << "WHY_LOG_INFO_WITH_STREAM" << " " << "test successful";
    WHY_LOG_WARN_WITH_STREAM(test_logger) << "WHY_LOG_WARN_WITH_STREAM" << " " << "test successful";
    WHY_LOG_ERROR_WITH_STREAM(test_logger) << "WHY_LOG_ERROR_WITH_STREAM" << " " << "test successful";
    WHY_LOG_FATAL_WITH_STREAM(test_logger) << "WHY_LOG_FATAL_WITH_STREAM" << " " << "test successful";
}

void test_fmt_with_StdOutLogAppender() {
    test_logger->ClearAppenders();
    test_logger->AddAppender(std::make_shared<StdOutLogAppender>());
    test_logger->SetLogLevel(LogLevel::WARN);

    WHY_LOG_DEBUG(test_logger, "WHY_LOG_DEBUG test %s", "successful");
    WHY_LOG_INFO(test_logger, "WHY_LOG_INFO test %s", "successful");
    WHY_LOG_WARN(test_logger, "WHY_LOG_WARN test %s", "successful");
    WHY_LOG_ERROR(test_logger, "WHY_LOG_ERROR test %s", "successful");
    WHY_LOG_FATAL(test_logger, "WHY_LOG_FATAL test %s", "successful");
    
}

int main() {
    test_stream_with_StdOutLogAppender();
    test_fmt_with_StdOutLogAppender();
    return 0;
}