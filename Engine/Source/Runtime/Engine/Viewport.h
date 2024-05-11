#pragma once

class GameViewport
{
public:
	GameViewport(std::any hwnd, const RenderConfig& config);
	virtual ~GameViewport();

	RHIViewport* GetRHI() { return _ViewportRHI; }
private:
	RHIViewport* _ViewportRHI;
};