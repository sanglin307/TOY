#pragma once

#ifdef WINDOWS
#define CORE_API __declspec(dllexport)
#endif

#include <format>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <vector>
#include <string>
#include <ctime>
#include <cassert>
#include <any>
#include <set>
#include <map>
#include <array>

#include "Types.h"
#include "Log.h"
#include "Path.h"
#include "Platform.h"
