#pragma once

#ifdef WINDOWS
#define RHI_API __declspec(dllexport)
#endif

#include "VertexFormat.h"
#include "Reflection.h"
#include "Resource.h"
#include "Shader.h"
#include "Pipeline.h"
#include "Context.h"
#include "Device.h"
#include "RHI.h"
