#pragma once

#ifdef WINDOWS
#define NOMINMAX
#define CORE_API __declspec(dllexport)
#endif

#include <format>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <vector>
#include <string>
#include <string_view>
#include <ctime>
#include <any>
#include <set>
#include <map>
#include <array>
#include <unordered_map>
#include <bitset>

//core
#include "Types.h"
#include "Log.h"
#include "Hash.h"
#include "Path.h"
#include "Platform.h"
#include "Module.h"
