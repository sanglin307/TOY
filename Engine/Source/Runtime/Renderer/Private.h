#pragma once

#ifdef WINDOWS
#define RENDERER_API __declspec(dllexport)
#endif

#define HLSLPP_FEATURE_TRANSFORM
#include "hlsl++.h"
using namespace hlslpp;

#include "../Core/Public.h"
#include "../RHI/Public.h"
#include "../Engine/Public.h"

#include "MeshCommand.h"
#include "RenderScene.h"
#include "RenderPass.h"
#include "SceneRendering.h"
#include "Renderer.h"
