#pragma once

// macro define.
#ifdef WINDOWS
#define NOMINMAX
#define ENGINE_API __declspec(dllexport)
#endif

#include "glm.hpp"

// module dependency.
#include "../Core/Public.h"
#include "../RHI/Public.h"

// inner dependency.
#include "IRenderer.h"
#include "FrameRate.h"
#include "Image.h"
#include "Viewport.h"
#include "Engine.h"
#include "Input.h"
#include "Material.h"
#include "Mesh.h"
#include "Primitive.h"
#include "World.h"