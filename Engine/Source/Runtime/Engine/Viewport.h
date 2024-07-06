#pragma once

class GameViewport
{
public:
	GameViewport(std::any hwnd, const RenderConfig& config);
	virtual ~GameViewport();

	hlslpp::uint2 GetSize() {
		return _ViewportSize;
	}

	Swapchain* GetRHI() { return _SwapchainRHI; }
private:
	Swapchain* _SwapchainRHI;
	hlslpp::uint2 _ViewportSize;
};