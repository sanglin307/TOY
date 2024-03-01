#include "RenderPath.h"
#include "Renderer.h"
#include "../DX12/Device.h"

void RenderPath::Init()
{
	const RenderConfig& Config = Renderer::Instance().Config();
	if (Config.API == RenderAPI::DX12)
	{
		_Device = new DX12Device;
	}
}

void RenderPath::Destroy()
{
	if (_SwapChain)
	{
		delete _SwapChain;
		_SwapChain = nullptr;
	}

	if (_Device)
	{
		delete _Device;
		_Device = nullptr;
	}
}