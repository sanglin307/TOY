#include "Platform.h"

#ifdef WINDOWS
void WindowsUtils::UTF16ToUTF8(const std::wstring& utf16, std::string& utf8)
{
    i32 utf8Length = WideCharToMultiByte(CP_UTF8, 0, utf16.data(), -1, nullptr, 0, nullptr, nullptr);
    utf8.resize(utf8Length);
    WideCharToMultiByte(CP_UTF8, 0, utf16.data(), -1, utf8.data(), utf8Length, nullptr, nullptr);
}

void WindowsUtils::UTF8ToUTF16(const std::string& utf8, std::wstring& utf16)
{
    i32 utf16Length = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
    utf16.resize(utf16Length);
    MultiByteToWideChar(CP_UTF8, 0, utf8.data(), -1, utf16.data(), utf16Length);
}

void WindowsUtils::TimeNow(std::string_view str)
{
    assert(str.length() >= 32);
    time_t current_time = time(nullptr);
    struct tm local_time;
    localtime_s(&local_time, &current_time);
    std::strftime(const_cast<char*>(&str[0]), str.length(), "%F %T", &local_time);
}

#endif