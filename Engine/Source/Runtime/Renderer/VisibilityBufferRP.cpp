#include "Private.h"

void VisibilityBufferRP::Init()
{
	RenderPath::Init();

	const RenderConfig& Config = Renderer::Instance().Config();
	_DirectQueue = _Device->CreateCommandQueue(CommandType::Direct);
	_CommandAllocator = _Device->CreateCommandAllocator(CommandType::Direct);

	SwapChain::Config sc = {
		.Width = Config.FrameWidth,
		.Height = Config.FrameHeight,
		.SampleCount = 1,
		.SampleQuality = 0,
		.BufferCount = Config.FrameCount,
		.Format = PixelFormat::R8G8B8A8_UNORM,
	};

	_SwapChain = _Device->CreateSwapChain(sc, _DirectQueue, Renderer::Instance().HWND());
	_FrameIndex = _SwapChain->CurrentFrameIndex();
}
void VisibilityBufferRP::Render()
{

}

void VisibilityBufferRP::Destroy()
{
	
	if (_DirectQueue)
	{
		delete _DirectQueue;
		_DirectQueue = nullptr;
	}

	if (_CommandAllocator)
	{
		delete _CommandAllocator;
		_CommandAllocator = nullptr;
	}
 
	RenderPath::Destroy();
}