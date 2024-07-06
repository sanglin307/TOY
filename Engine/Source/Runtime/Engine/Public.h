#pragma once

#ifdef WINDOWS
#define ENGINE_API __declspec(dllexport)
#endif

#include "IRenderer.h"
#include "FrameRate.h"
#include "Component.h"
#include "Input.h"
#include "Light.h"
#include "Image.h"
#include "Viewport.h"
#include "Engine.h"
#include "Material.h"
#include "Primitive.h"
#include "Mesh.h"
#include "Layer.h"
#include "glTFLoader.h"