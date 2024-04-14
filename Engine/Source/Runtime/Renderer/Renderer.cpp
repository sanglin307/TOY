#include "Private.h"

IMPLEMENT_MODULE(RendererModule)
RendererModule* GRendererModule = nullptr;

void RendererModule::Init()
{
}

RendererModule& RendererModule::Instance()
{
	check(GRendererModule);
	return *GRendererModule;
}

void RendererModule::CreateRenderer(std::any hwnd, const RenderConfig& config)
{
	GRendererModule = this;
	_HWND = hwnd;
	_Config = config;

	ModuleInterface* mi = nullptr;
	if (_Config.API == RenderAPI::DX12)
	{
		mi = ModuleManager::LoadModule("DX12");
	}

	check(mi);
	RHIModule* rhi = static_cast<RHIModule*>(mi);
	_Device = rhi->CreateDevice();
	_Device->Init();

	_CommandMgr = new CommandManager;
	_CommandMgr->Init(_Config.FrameCount);

	constexpr u32 GlobalDescriptorNumber = 2000;
	constexpr u32 SamplerDescriptorNumber = 500;
	constexpr u32 RVTDescriptorNumber = 64;
	constexpr u32 DSVDescriptorNumber = 32;

	_GlobalDescriptorHeap = RD.CreateDescriptorHeap(DescriptorType::CBV_SRV_UAV, GlobalDescriptorNumber, true);
	_SamplerDescriptorHeap = RD.CreateDescriptorHeap(DescriptorType::Sampler, SamplerDescriptorNumber, true);
	_RVTDescriptorHeap = RD.CreateDescriptorHeap(DescriptorType::RVT, RVTDescriptorNumber, false);
	_DSVDescriptorHeap = RD.CreateDescriptorHeap(DescriptorType::DSV, DSVDescriptorNumber, false);

	SwapChain::Config sc = {
		.Width = _Config.FrameWidth,
		.Height = _Config.FrameHeight,
		.SampleCount = 1,
		.SampleQuality = 0,
		.BufferCount = _Config.FrameCount,
		.Format = PixelFormat::R8G8B8A8_UNORM,
	};

	_SwapChain = RD.CreateSwapChain(sc, _CommandMgr->DirectCommandQueue(), _RVTDescriptorHeap,_HWND);
	_FrameIndex = _SwapChain->CurrentFrameIndex();

	_Scene = new RenderScene;
	_Scene->Init();

	_Fence = RD.CreateFence(_Config.FrameCount);
	_Fence->Wait(_FrameIndex, _CommandMgr->DirectCommandQueue());

}

void RendererModule::PreRender()
{
	RC.Prepare(_FrameIndex);
}

void RendererModule::PostRender()
{
	RC.End(_SwapChain->RenderTarget(_FrameIndex));

	_SwapChain->Present(true);

	u32 nextFrameIndex = _SwapChain->CurrentFrameIndex();
	_Fence->Advance(_FrameIndex, nextFrameIndex, _CommandMgr->DirectCommandQueue());
	_FrameIndex = nextFrameIndex;
}

void RendererModule::Render()
{
	PreRender();

	RC.SetViewport(Viewport{
		.Width = _Config.FrameWidth,
		.Height = _Config.FrameHeight
		});

	RC.SetScissorRect(0, 0, _Config.FrameWidth, _Config.FrameHeight);

	std::vector<Texture2DResource*> renderTargets;
	renderTargets.push_back(_SwapChain->RenderTarget(_FrameIndex));
	RC.SetRenderTargets(renderTargets, nullptr);

	std::array<float, 4> colors = { 0.0f, 1.f, 0.4f, 1.0f };
	RC.ClearRenderTarget(_SwapChain->RenderTarget(_FrameIndex), colors);


	PostRender();
}

void RendererModule::Destroy()
{
	_Scene->Destroy();
	delete _Scene;
	_Scene = nullptr;

	delete _GlobalDescriptorHeap;
	_GlobalDescriptorHeap = nullptr;

	delete _SamplerDescriptorHeap; 
	_SamplerDescriptorHeap = nullptr;

	delete _RVTDescriptorHeap;
	_RVTDescriptorHeap = nullptr;

	delete _DSVDescriptorHeap;
	_DSVDescriptorHeap = nullptr;

	delete _Fence;
	_Fence = nullptr;

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