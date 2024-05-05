#include "Private.h"

Viewport::Viewport(std::any hwnd, const RenderConfig& config)
{
	_ViewportRHI = new RHIViewport(config.FrameCount, config.FrameWidth, config.FrameHeight, hwnd);
}

Viewport::~Viewport()
{
	delete _ViewportRHI;
}
