#pragma once

#ifdef WINDOWS
#define RENDERER_API __declspec(dllexport)
#endif

#include "RenderPath.h"
#include "RenderScene.h"
#include "Resource.h"
#include "Command.h"
#include "Pipeline.h"
#include "Renderer.h"
#include "Device.h"
