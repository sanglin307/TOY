#pragma once

#ifdef WINDOWS
#define RENDERER_API __declspec(dllexport)
#endif


#include "../Core/Public.h"
#include "../RenderCore/Public.h"
#include "../Engine/Public.h"
#include "../ShaderCompiler/Public.h"

#include "PipelineCache.h"
#include "RenderScene.h"
#include "Renderer.h"
