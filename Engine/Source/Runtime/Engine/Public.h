#pragma once

#ifdef WINDOWS
#define ENGINE_API __declspec(dllexport)
#endif

#include "RenderScene.h"
#include "Input.h"
#include "FrameRate.h"
#include "Viewport.h"
#include "Engine.h"
#include "Material.h"
#include "Mesh.h"
#include "World.h"