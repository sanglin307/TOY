#pragma once

#ifdef WINDOWS
#define RHI_API __declspec(dllexport)
#endif

#include "Reflection.h"
#include "Resource.h"
#include "VertexFormat.h"
#include "Shader.h"
#include "DynamicRingBuffer.h"
#include "Pipeline.h"
#include "Context.h"
#include "Device.h"
#include "RHI.h"
