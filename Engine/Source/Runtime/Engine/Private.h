#pragma once

// macro define.
#ifdef WINDOWS
#define ENGINE_API __declspec(dllexport)
#endif


// module dependency.
#include "../Core/Public.h"
#include "../RHI/Public.h"
#include "../ShaderCompiler/Public.h"

// inner dependency.
#include "FrameRate.h"
#include "Viewport.h"
#include "Engine.h"
#include "Input.h"
#include "Material.h"
#include "StaticMesh.h"
#include "World.h"