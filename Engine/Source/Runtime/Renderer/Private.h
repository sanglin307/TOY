#pragma once

#ifdef WINDOWS
#define RENDERER_API __declspec(dllexport)
#endif


#include "../Core/Public.h"
#include "../RHI/Public.h"
#include "../Engine/Public.h"

#include "PipelineCache.h"
#include "RenderScene.h"
#include "Renderer.h"
