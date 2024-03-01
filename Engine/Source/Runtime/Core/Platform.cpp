#include "Platform.h"

#ifdef WINDOWS
std::string WindowsUtils::UTF16ToUTF8(const std::wstring& utf16)
{
    std::string utf8;
    i32 utf8Length = WideCharToMultiByte(CP_UTF8, 0, utf16.data(), -1, nullptr, 0, nullptr, nullptr);
    utf8.resize(utf8Length);
    WideCharToMultiByte(CP_UTF8, 0, utf16.data(), -1, utf8.data(), utf8Length, nullptr, nullptr);
    return utf8;
}

std::wstring WindowsUtils::UTF8ToUTF16(const std::string& utf8)
{
    std::wstring utf16;
    i32 utf16Length = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
    utf16.resize(utf16Length);
    MultiByteToWideChar(CP_UTF8, 0, utf8.data(), -1, utf16.data(), utf16Length);
    return utf16;
}

std::string WindowsUtils::TimeNowString()
{
    time_t current_time = time(nullptr);
    struct tm local_time;
    localtime_s(&local_time, &current_time);
    char buf[32];
    std::strftime(buf, 32, "%F %T", &local_time);
    return buf;
}

std::string WindowsUtils::DateNowString()
{
    time_t current_time = time(nullptr);
    struct tm local_time;
    localtime_s(&local_time, &current_time);
    char buf[32];
    std::strftime(buf, 32, "%F", &local_time);
    return buf;
}

#endif