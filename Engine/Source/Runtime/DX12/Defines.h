#pragma once

#ifdef WINDOWS
#define DX12_API __declspec(dllexport)
#endif


#include <d3d12.h>
#include <dxgi1_6.h>
#include <assert.h>

#include <vector>
#include <string>