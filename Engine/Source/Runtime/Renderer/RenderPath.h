#pragma once

#include "Defines.h"
#include "Device.h"
#include "Resource.h"


class RenderPath
{
public:
	virtual void Init();
	virtual void Render() = 0;
	virtual void Destroy();

protected:
	RenderDevice* _Device = nullptr;
	SwapChain* _SwapChain = nullptr;
	u32 _FrameIndex = 0;

};


