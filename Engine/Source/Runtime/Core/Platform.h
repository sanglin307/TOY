#pragma once

#ifdef WINDOWS

class CORE_API WindowsUtils
{
public:
	static std::string UTF16ToUTF8(const std::wstring& utf16);
	static std::wstring UTF8ToUTF16(const std::string& utf8);
	static std::string TimeNowString();
	static std::string DateNowString();
	static void OutputDebug(const std::string& str);
	
};

using PlatformUtils = WindowsUtils;

#endif
