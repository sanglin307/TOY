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

#define HLSLPP_FEATURE_TRANSFORM
#include "hlsl++.h"
using namespace hlslpp;

#include "../Core/Public.h"
#include "../RHI/Public.h"

#include "Device.h"
#include "Resource.h"
#include "Command.h"
#include "Pipeline.h"
#include "RHI.h"