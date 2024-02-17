#pragma once

#include "Defines.h"
#include "Command.h"
#include "Resource.h"
#include "Pipeline.h"

class RenderDevice
{
public:
	virtual void Init() = 0;
	virtual void Destroy() = 0;

	virtual CommandQueue* CreateCommandQueue(const CommandType type) = 0;
	virtual CommandAllocator* CreateCommandAllocator(const CommandType type) = 0;
	virtual SwapChain* CreateSwapChain(const SwapChain::Config& config, CommandQueue* queue, void* hwnd = nullptr) = 0;
	virtual RootSignature* CreateRootSignature() = 0;

};
