#pragma once

#pragma once

#ifdef WINDOWS
#define SHADER_API __declspec(dllexport)

#include <wrl/client.h>
using namespace Microsoft::WRL;


#include "dxcapi.h"    // load from thirdparty folder.
#include "d3d12shader.h"

#endif


#include "../Core/Public.h"


#include "Compiler.h"