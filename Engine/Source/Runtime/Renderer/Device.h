#pragma once

#include "Defines.h"
#include "Command.h"
#include "Resource.h"

class RenderDevice
{
public:
	virtual void Init() = 0;
	virtual void Destroy() = 0;

	virtual CommandQueue* CreateCommandQueue(const CommandQueue::Type type) = 0;
	virtual SwapChain* CreateSwapChain(const SwapChain::Config& config, CommandQueue* queue, void* hwnd = nullptr) = 0;

};
