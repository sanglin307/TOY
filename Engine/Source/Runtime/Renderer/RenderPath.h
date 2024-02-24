#pragma once

#include "Defines.h"
#include "Device.h"
#include "Resource.h"


class RenderPath
{
public:
	virtual void Init() = 0;
	virtual void Render() = 0;
	virtual void Destroy() = 0;

};

class VisibilityBufferRP : public RenderPath
{
public:
	virtual void Init() override;
	virtual void Render() override;
	virtual void Destroy() override;

private:

	RenderDevice* _Device = nullptr;
	CommandQueue* _DirectQueue = nullptr;
	SwapChain* _SwapChain = nullptr;
	CommandAllocator* _CommandAllocator = nullptr;
	u32 _FrameIndex = 0;
};
