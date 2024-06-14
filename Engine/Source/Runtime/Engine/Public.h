#pragma once

#ifdef WINDOWS
#define ENGINE_API __declspec(dllexport)
#endif

#include "IRenderer.h"
#include "Input.h"
#include "FrameRate.h"
#include "Viewport.h"
#include "Image.h"
#include "Engine.h"
#include "Material.h"
#include "Mesh.h"
#include "Primitive.h"
#include "World.h"