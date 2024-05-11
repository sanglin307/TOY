#pragma once

class GameViewport
{
public:
	GameViewport(std::any hwnd, const RenderConfig& config);
	virtual ~GameViewport();

	Swapchain* GetRHI() { return _SwapchainRHI; }
private:
	Swapchain* _SwapchainRHI;
};