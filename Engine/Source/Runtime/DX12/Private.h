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


#include "../Core/Public.h"
#include "../RenderCore/Public.h"
#include "../RHI/Public.h"

#include "Module.h"
#include "Device.h"
#include "Resource.h"
#include "Command.h"
#include "Context.h"
#include "Pipeline.h"