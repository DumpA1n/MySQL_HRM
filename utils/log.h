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
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    std::ostringstream oss;
    oss << (now->tm_year + 1900) << '-'
        << (now->tm_mon + 1) << '-'
        << now->tm_mday << ' '
        << now->tm_hour << ':'
        << now->tm_min << ':'
        << now->tm_sec;
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
    void LOGE(std::string info) {
        infoV.push_back(info);
        infoS += getTimeStamp() + " [E] " + info;
        infoS += "\n";
    }
    void clear() {
        infoV.clear();
        infoS.clear();
    }
} mlog;

#define LOGI(info) mlog.LOGI(info)
#define LOGE(info) mlog.LOGE(info)



#endif // LOG_H