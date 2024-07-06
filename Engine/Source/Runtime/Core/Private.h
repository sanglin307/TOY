#pragma once

#ifdef WINDOWS
#define CORE_API __declspec(dllexport)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN   
#endif

#define NOMINMAX

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
#include <map>
#include <string>
#include <ctime>
#include <any>

#define HLSLPP_FEATURE_TRANSFORM
#include "hlsl++.h"

// iner 
#include "Types.h"
#include "Log.h"
#include "Math.h"
#include "Hash.h"
#include "Path.h"
#include "Platform.h"
#include "Module.h"