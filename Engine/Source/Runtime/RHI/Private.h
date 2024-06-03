#pragma once

#ifdef WINDOWS
#define RHI_API __declspec(dllexport)
#endif

#include "../Core/Public.h"

#include "Reflection.h"
#include "Shader.h"
#include "Compiler.h"
#include "Resource.h"
#include "Pipeline.h"
#include "Context.h"
#include "Descriptor.h"
#include "Device.h"
#include "RHI.h"