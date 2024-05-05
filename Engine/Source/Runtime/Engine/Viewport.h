#pragma once

class Viewport
{
public:
	Viewport(std::any hwnd, const RenderConfig& config);
	virtual ~Viewport();

private:
	RHIViewport* _ViewportRHI;
};