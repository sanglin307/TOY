#pragma once

// macro define.
#ifdef WINDOWS
#define ENGINE_API __declspec(dllexport)
#endif


//third party
#define TOML_EXCEPTIONS 0
#include "toml++/toml.hpp"

#include "hlsl++.h"

// module dependency.
#include "../Core/Public.h"
#include "../RHI/Public.h"
#include "../ShaderCompiler/Public.h"


// inner dependency.
#include "RenderScene.h"
#include "FrameRate.h"
#include "Viewport.h"
#include "Engine.h"
#include "Input.h"
#include "Material.h"
#include "Mesh.h"
#include "World.h"