#pragma once

#define NOMINMAX
#define DX12_API __declspec(dllexport)

#include <wrl/client.h>
using namespace Microsoft::WRL;


#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgi1_3.h>
#ifdef _DEBUG
#include <dxgidebug.h>
#endif

#ifdef NV_Aftermath
#pragma warning(disable: 4819)
#include <GFSDK_Aftermath.h>
#include <GFSDK_Aftermath_GpuCrashDump.h>
#include <GFSDK_Aftermath_GpuCrashDumpDecoding.h>
#endif

#define USE_PIX
#include "WinPixEventRuntime/pix3.h"

#include "DirectXTex.h"

#include "MemoryAllocator/D3D12MemAlloc.h"

#define HLSLPP_FEATURE_TRANSFORM
#include "hlsl++.h"
using namespace hlslpp;


#include "../Core/Public.h"
#include "../RHI/Public.h"
#include "CommonStruct.h"

#include "../Engine/Public.h"



#include "Device.h"
#include "Resource.h"
#include "Command.h"
#include "Pipeline.h"
#include "RHI.h"