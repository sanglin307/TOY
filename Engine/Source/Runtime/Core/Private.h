#pragma once

#ifdef WINDOWS
#define CORE_API __declspec(dllexport)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN   
#endif

#include <windows.h>
#include <windowsx.h>
#include <WinUser.h>

#endif

// std lib
#include <format>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <vector>
#include <string>
#include <ctime>
#include <cassert>


// iner 
#include "Types.h"
#include "Log.h"
#include "Path.h"
#include "Platform.h"