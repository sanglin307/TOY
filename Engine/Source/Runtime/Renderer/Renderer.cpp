#include "Renderer.h"
#include "../DX12/Device.h"
#include "../Engine/Engine.h"

void Renderer::Init(const GameEngine::Config& config, void* hwnd)
{
	if (config.API == RenderAPI::DX12)
	{
		_Device = new DX12Device;
	}


	_Device->Init();
	_DirectQueue = _Device->CreateCommandQueue(CommandQueue::Type::Direct);

	SwapChain::Config sc = {
		.Width = config.Width,
		.Height = config.Height,
		.Format = GraphicFormat::R8G8B8A8_UNORM,
		.SampleCount = config.SampleCount,
		.SampleQuality = 0,
		.BufferCount = config.BufferCount,
	};
	_SwapChain = _Device->CreateSwapChain(sc, _DirectQueue, hwnd);
	_FrameIndex = _SwapChain->CurrentFrameIndex();
}

void Renderer::Destroy()
{
	if (_SwapChain)
	{
		delete _SwapChain;
		_SwapChain = nullptr;
	}

	if (_DirectQueue)
	{
		delete _DirectQueue;
		_DirectQueue = nullptr;
	}

	if (_Device)
	{
		_Device->Destroy();
		delete _Device;
		_Device = nullptr;
	}
}