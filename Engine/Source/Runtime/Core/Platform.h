#pragma once

#include "Defines.h"
#include "Types.h"

#include <vector>
#include <string>

#ifdef WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN   
#endif

#include <windows.h>
#include <windowsx.h>
#include <WinUser.h>

class CORE_API WindowsUtils
{
public:
	static void UTF16ToUTF8(const std::wstring& utf16, std::string& utf8);
	static void UTF8ToUTF16(const std::string&, std::wstring& utf16);
	
};

#endif

typedef WindowsUtils PlatformUtils;
