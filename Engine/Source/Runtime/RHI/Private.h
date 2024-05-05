#pragma once

#ifdef WINDOWS
#define RHI_API __declspec(dllexport)
#endif

#include "../Core/Public.h"
#include "../RenderCore/Public.h"

#include "Device.h"
#include "Context.h"
#include "RHI.h"