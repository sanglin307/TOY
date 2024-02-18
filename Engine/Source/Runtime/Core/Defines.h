#pragma once

#ifdef WINDOWS
#define CORE_API __declspec(dllexport)
#endif

#include <vector>
#include <string>
#include <string_view>
#include <ctime>
#include <cassert>