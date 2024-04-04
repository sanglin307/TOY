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
		ModuleInterface* mi = ModuleManager::LoadModule("DX12");
		assert(mi);
		RHIModule* rhi = (RHIModule*)mi;
		_Device = rhi->CreateDevice();
		_Device->InitPixelFormat();
		_Device->Init();
	}

	_CommandMgr = new CommandManager;
	_CommandMgr->Init();

	constexpr u32 GlobalDescriptorNumber = 2000;
	constexpr u32 SamplerDescriptorNumber = 500;
	constexpr u32 RVTDescriptorNumber = 64;
	constexpr u32 DSVDescriptorNumber = 32;

	_GlobalDescriptorHeap = RHI.CreateDescriptorHeap(DescriptorType::CBV_SRV_UAV, GlobalDescriptorNumber, true);
	_SamplerDescriptorHeap = RHI.CreateDescriptorHeap(DescriptorType::Sampler, SamplerDescriptorNumber, true);
	_RVTDescriptorHeap = RHI.CreateDescriptorHeap(DescriptorType::RVT, RVTDescriptorNumber, false);
	_DSVDescriptorHeap = RHI.CreateDescriptorHeap(DescriptorType::DSV, DSVDescriptorNumber, false);

	SwapChain::Config sc = {
		.Width = _Config.FrameWidth,
		.Height = _Config.FrameHeight,
		.SampleCount = 1,
		.SampleQuality = 0,
		.BufferCount = _Config.FrameCount,
		.Format = PixelFormat::R8G8B8A8_UNORM,
	};

	_SwapChain = RHI.CreateSwapChain(sc, _CommandMgr->DirectQueue(), Renderer::Instance().HWND());
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

	delete _GlobalDescriptorHeap;
	_GlobalDescriptorHeap = nullptr;

	delete _SamplerDescriptorHeap; 
	_SamplerDescriptorHeap = nullptr;

	delete _RVTDescriptorHeap;
	_RVTDescriptorHeap = nullptr;

	delete _DSVDescriptorHeap;
	_DSVDescriptorHeap = nullptr;

	if (_SwapChain)
	{
		delete _SwapChain;
		_SwapChain = nullptr;
	}

	_CommandMgr->Destroy();
	delete _CommandMgr;
	_CommandMgr = nullptr;

	RootSignatureManager::Destroy();
	PipelineManager::Destroy();
	ShaderManager::Destroy();
 
	_Device->Destroy();
	delete _Device;
	_Device = nullptr;

	
}