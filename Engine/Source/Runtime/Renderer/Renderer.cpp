#include "Private.h"

Renderer& Renderer::Instance()
{
	static Renderer Inst;
	return Inst;
}

void Renderer::ParseCmds(const std::set<std::string>& cmds)
{
	_Config = {
		.API = RenderAPI::DX12,
		.RenderPath = RenderPathType::VisibilityBuffer,
		.FrameCount = 3,
		.FrameWidth = 1280,
		.FrameHeight = 800,
	};
}
 

void Renderer::Init(std::any hwnd)
{
	_HWND = hwnd;
 
	if (_Config.API == RenderAPI::DX12)
	{
		_Device = CreateDX12Device();
		_Device->InitPixelFormat();
		_Device->Init();
	}

	_DirectCommandQueue = RHI.CreateCommandQueue(CommandType::Direct);
	_DirectCommandAllocator = RHI.CreateCommandAllocator(CommandType::Direct);
	_DirectCommandList = RHI.CreateCommandList(_DirectCommandAllocator, CommandType::Direct);

	_ComputeCommandQueue = RHI.CreateCommandQueue(CommandType::Compute);
	_ComputeCommandAllocator = RHI.CreateCommandAllocator(CommandType::Compute);
	_ComputeCommandList = RHI.CreateCommandList(_ComputeCommandAllocator, CommandType::Compute);

	_CopyCommandQueue = RHI.CreateCommandQueue(CommandType::Copy);
	_CopyCommandAllocator = RHI.CreateCommandAllocator(CommandType::Copy);
	_CopyCommandList = RHI.CreateCommandList(_CopyCommandAllocator, CommandType::Copy);

	SwapChain::Config sc = {
		.Width = _Config.FrameWidth,
		.Height = _Config.FrameHeight,
		.SampleCount = 1,
		.SampleQuality = 0,
		.BufferCount = _Config.FrameCount,
		.Format = PixelFormat::R8G8B8A8_UNORM,
	};

	_SwapChain = RHI.CreateSwapChain(sc, _DirectCommandQueue, Renderer::Instance().HWND());
	_FrameIndex = _SwapChain->CurrentFrameIndex();

	_Scene = new RenderScene;
	_Scene->Init();

	if (_Config.RenderPath == RenderPathType::VisibilityBuffer)
	{
		_RenderPath = new VisibilityBufferRP;
		_RenderPath->Init();
	}
}

void Renderer::Render()
{
	_RenderPath->Render();
}

void Renderer::Destroy()
{
	_Scene->Destroy();
	delete _Scene;
	_Scene = nullptr;

	_RenderPath->Destroy();
	delete _RenderPath;
	_RenderPath = nullptr;

	if (_SwapChain)
	{
		delete _SwapChain;
		_SwapChain = nullptr;
	}

	RootSignatureManager::Destroy();
	PipelineManager::Destroy();
	ShaderManager::Destroy();

	if (_CopyCommandList)
	{
		delete _CopyCommandList;
		_CopyCommandList = nullptr;
	}

	if (_CopyCommandQueue)
	{
		delete _CopyCommandQueue;
		_CopyCommandQueue = nullptr;
	}

	if (_CopyCommandAllocator)
	{
		delete _CopyCommandAllocator;
		_CopyCommandAllocator = nullptr;
	}

	if (_ComputeCommandList)
	{
		delete _ComputeCommandList;
		_ComputeCommandList = nullptr;
	}

	if (_ComputeCommandQueue)
	{
		delete _ComputeCommandQueue;
		_ComputeCommandQueue = nullptr;
	}

	if (_DirectCommandAllocator)
	{
		delete _DirectCommandAllocator;
		_DirectCommandAllocator = nullptr;
	}

	if (_DirectCommandList)
	{
		delete _DirectCommandList;
		_DirectCommandList = nullptr;
	}

	if (_DirectCommandQueue)
	{
		delete _DirectCommandQueue;
		_DirectCommandQueue = nullptr;
	}

	if (_DirectCommandAllocator)
	{
		delete _DirectCommandAllocator;
		_DirectCommandAllocator = nullptr;
	}

	_Device->Destroy();
	delete _Device;
	_Device = nullptr;

	
}