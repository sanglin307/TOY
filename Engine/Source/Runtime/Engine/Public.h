#pragma once

#ifdef WINDOWS
#define ENGINE_API __declspec(dllexport)
#endif

#include "Input.h"
#include "FrameRate.h"
#include "Viewport.h"
#include "Engine.h"
#include "Material.h"
#include "StaticMesh.h"
#include "World.h"