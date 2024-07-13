#pragma once

class GameViewport
{
public:
	GameViewport(std::any hwnd, const RenderConfig& config);
	virtual ~GameViewport();

	uint2 GetSize() 
	{
		return _ViewportSize;
	}

	void OnResize(u32 width, u32 height);
	Swapchain* GetRHI() { return _SwapchainRHI; }
private:
	Swapchain* _SwapchainRHI;
	uint2 _ViewportSize;
};