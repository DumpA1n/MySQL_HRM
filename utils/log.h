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

std::string getTimeStamp() {
    // 获取当前时间点
    auto now = std::chrono::system_clock::now();
    
    // 转换为 time_t 类型以格式化日期和时间
    auto timeT = std::chrono::system_clock::to_time_t(now);
    
    // 获取当前时间的毫秒部分
    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;

    // 格式化日期和时间
    std::tm tm = *std::localtime(&timeT);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%m-%d %H:%M:%S"); // 格式化 MM-DD HH:MM:SS
    oss << '.' << std::setfill('0') << std::setw(3) << millis; // 添加毫秒部分

    return oss.str();
}

class Log
{
public:
    /* data */
    std::vector<std::string> infoV;
    std::string infoS;
    void LOGI(std::string info) {
        infoV.push_back(info);
        infoS += getTimeStamp() + " [I] " + info;
        infoS += "\n";
    }
    void clear() {
        infoV.clear();
        infoS.clear();
    }
} mlog;

#define LOGI(info) mlog.LOGI(info)



#endif // LOG_H