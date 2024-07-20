#pragma once

#ifdef WINDOWS
#define RHI_API __declspec(dllexport)
#endif

#define HLSLPP_FEATURE_TRANSFORM
#include "hlsl++.h"
using namespace hlslpp;

#include "../Core/Public.h"

#include "Reflection.h"
#include "Resource.h"
#include "Shader.h"
#include "VertexFormat.h"
#include "Compiler.h"
#include "Pipeline.h"
#include "Context.h"
#include "Device.h"
#include "RHI.h"