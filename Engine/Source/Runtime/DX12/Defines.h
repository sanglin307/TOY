#pragma once

#define DX12_API __declspec(dllexport)

#include <wrl/client.h>
using namespace Microsoft::WRL;


#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgi1_3.h>
#ifdef _DEBUG
#include <dxgidebug.h>
#endif

#include <assert.h>


#include <vector>
#include <string>
#include <any>