#pragma once

// macro define.
#ifdef WINDOWS
#define ENGINE_API __declspec(dllexport)
#endif


// module dependency.
#include "../Core/Public.h"
#include "../RenderCore/Public.h"

// inner dependency.
#include "FrameRate.h"
#include "Engine.h"
#include "Input.h"
#include "StaticMesh.h"
#include "World.h"