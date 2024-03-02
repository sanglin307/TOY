#pragma once

#ifdef WINDOWS
#define ENGINE_API __declspec(dllexport)
#endif

#include "Input.h"
#include "FrameRate.h"
#include "Engine.h"
