#pragma once

#include "Defines.h"
#include "Command.h"
#include "Resource.h"
#include "Pipeline.h"

class RenderDevice
{
public:

	virtual ~RenderDevice() {};
	virtual std::any Handle() { return nullptr; };
	virtual CommandQueue* CreateCommandQueue(const CommandType type) = 0;
	virtual CommandAllocator* CreateCommandAllocator(const CommandType type) = 0;
	virtual SwapChain* CreateSwapChain(const SwapChain::Config& config, CommandQueue* queue, const std::any hwnd = nullptr) = 0;
	virtual RootSignature* CreateRootSignature() = 0;

};
