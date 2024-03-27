#pragma once

class RenderDevice;
class SwapChain;

class RenderPath
{
public:
	virtual void Init();
	virtual void Render() = 0;
	virtual void Destroy();

protected:
	SwapChain* _SwapChain = nullptr;
	u32 _FrameIndex = 0;

};


