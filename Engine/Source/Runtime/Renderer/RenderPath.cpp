#include "Private.h"

void RenderPath::Init()
{
}

void RenderPath::Destroy()
{
	if (_SwapChain)
	{
		delete _SwapChain;
		_SwapChain = nullptr;
	}
}