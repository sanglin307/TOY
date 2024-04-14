#pragma once

#ifdef WINDOWS
#define RENDERCORE_API __declspec(dllexport)
#endif

#include "../Core/Public.h"

#include "Config.h"
#include "Module.h"
#include "Reflection.h"
#include "Shader.h"
#include "Resource.h"
#include "Pipeline.h"
#include "Command.h"
#include "Device.h"
#include "RHI.h"
