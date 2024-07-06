#pragma once

// macro define.
#ifdef WINDOWS
#define NOMINMAX
#define ENGINE_API __declspec(dllexport)
#endif

#define HLSLPP_FEATURE_TRANSFORM
#include "hlsl++.h"
using namespace hlslpp;


// module dependency.
#include "../Core/Public.h"
#include "../RHI/Public.h"

// inner dependency.
#include "IRenderer.h"
#include "FrameRate.h"
#include "Component.h"
#include "Node.h"
#include "Input.h"
#include "Camera.h"
#include "Light.h"
#include "Image.h"
#include "Viewport.h"
#include "Engine.h"
#include "Material.h"
#include "Primitive.h"
#include "Mesh.h"
#include "Layer.h"
#include "glTFLoader.h"
#include "World.h"