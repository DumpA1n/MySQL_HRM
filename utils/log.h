#pragma once
#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <sstream>
#include <vector>
#include <string>
#include <mutex>
#include <cstdarg>

class Log {
public:
    enum LogLevel {
        INFO,
        ERROR
    };

    Log() : logLevel(INFO) {}

    void add(LogLevel level, const char* format, va_list args) {
        std::lock_guard<std::mutex> lock(mutex_);
        char buffer[1024]; // 可调整缓冲区大小
        vsnprintf(buffer, sizeof(buffer), format, args);

        std::string logEntry = getTimeStamp() + " " + levelToString(level) + " " + buffer;
        logs.push_back(logEntry);
        std::cout << logEntry << std::endl; // 可选：输出到控制台
    }

    void clear() {
        logs.clear();
    }

    std::vector<std::string> get() {
        return logs;
    }

    void LOGI(const char* format, ...) {
        va_list args;
        va_start(args, format);
        add(INFO, format, args); // 直接传递原始的 va_list
        va_end(args);
    }

    void LOGE(const char* format, ...) {
        va_list args;
        va_start(args, format);
        add(ERROR, format, args); // 直接传递原始的 va_list
        va_end(args);
    }

private:
    std::mutex mutex_;
    std::vector<std::string> logs; // 日志记录
    LogLevel logLevel;             // 当前日志级别

    // 获取时间戳
    std::string getTimeStamp() const {
        std::time_t now = std::time(nullptr);
        char buf[20];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        return std::string(buf);
    }

    // 转换日志级别为字符串
    std::string levelToString(LogLevel level) const {
        switch (level) {
        case INFO: return "[I]";
        case ERROR: return "[E]";
        default: return "[?]";
        }
    }
};

// 全局日志实例
Log mlog;

// 定义快捷宏
#define LOGI(format, ...) mlog.LOGI(format, ##__VA_ARGS__)
#define LOGE(format, ...) mlog.LOGE(format, ##__VA_ARGS__)



#endif // LOG_H