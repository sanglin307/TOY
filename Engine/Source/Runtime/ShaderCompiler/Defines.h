#pragma once

#pragma once

#ifdef WINDOWS
#define SHADER_API __declspec(dllexport)

#include <d3d12.h>
#include <dxcapi.h>
#include <d3d12shader.h>

#endif

#include <assert.h>
#include <vector>
#include <string>