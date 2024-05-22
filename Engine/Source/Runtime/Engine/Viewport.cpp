#include "Private.h"

GameViewport::GameViewport(std::any hwnd, const RenderConfig& config)
{
	RenderDevice* device = GameEngine::Instance().GetRHI().GetDevice();
	_SwapchainRHI = device->CreateSwapchain(Swapchain::Desc{
		.Width = config.FrameWidth,
		.Height = config.FrameHeight,
		.Format = PixelFormat::R8G8B8A8_UNORM,
		.HWND = hwnd,
		});
}

GameViewport::~GameViewport()
{
	delete _SwapchainRHI;
}
