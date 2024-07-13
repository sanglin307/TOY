#include "Private.h"

GameViewport::GameViewport(std::any hwnd, const RenderConfig& config)
{
	_ViewportSize.x = config.FrameWidth;
	_ViewportSize.y = config.FrameHeight;

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


void GameViewport::OnResize(u32 width, u32 height)
{
	_ViewportSize.x = width;
	_ViewportSize.y = height;

	RenderDevice* device = GameEngine::Instance().GetRHI().GetDevice();
	device->OnResize(_SwapchainRHI, width, height);
}