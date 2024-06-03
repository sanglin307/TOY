#pragma once

#ifdef WINDOWS
#define RENDERER_API __declspec(dllexport)
#endif

#include "../Core/Public.h"
#include "../RHI/Public.h"
#include "../Engine/Public.h"

#include "MeshCommand.h"
#include "RenderPass.h"
#include "RenderScene.h"
#include "Renderer.h"
