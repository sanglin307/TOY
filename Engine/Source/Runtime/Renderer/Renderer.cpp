#include "Private.h"

IMPLEMENT_MODULE(RendererModule)
RendererModule* GRendererModule = nullptr;

RendererModule& RendererModule::Instance()
{
	check(GRendererModule);
	return *GRendererModule;
}

IRHIModule& GetRHIModule(RenderAPI api)
{
	static IRHIModule* module = nullptr;
	if (module == nullptr)
	{
		if (api == RenderAPI::DX12)
		{
			module = static_cast<IRHIModule*>(ModuleManager::LoadModule("DX12"));
		}
		
	}

	return *module;
}

void RendererModule::Init(RenderAPI api)
{
	GRendererModule = this;

	IRHIModule& rhi = GetRHIModule(api);
	_Device = rhi.CreateDevice();
	_Context = rhi.CreateContext(SwapChainFrameCount);

	_Scene = new RenderScene;
	_Scene->Init();
}


void RendererModule::PostRender()
{
	RC.End(_SwapChain->RenderTarget(_FrameIndex));

	_SwapChain->Present(true);

	u32 nextFrameIndex = _SwapChain->CurrentFrameIndex();
	_Fence->Advance(_FrameIndex, nextFrameIndex, _CommandMgr->DirectCommandQueue());
	_FrameIndex = nextFrameIndex;
}

void RendererModule::Render(RHIViewport* viewport)
{
	GetRHIModule().PreRender();

	_Context->SetViewport(Viewport{
		.Width = _Config.FrameWidth,
		.Height = _Config.FrameHeight
		});

	RC.SetScissorRect(0, 0, _Config.FrameWidth, _Config.FrameHeight);

	std::vector<Texture2DResource*> renderTargets;
	renderTargets.push_back(_SwapChain->RenderTarget(_FrameIndex));
	RC.SetRenderTargets(renderTargets, nullptr);

	std::array<float, 4> colors = { 0.0f, 1.f, 0.4f, 1.0f };
	RC.ClearRenderTarget(_SwapChain->RenderTarget(_FrameIndex), colors);


	GetRHIModule().PostRender();
}

void RendererModule::Destroy()
{
	_Fence->Wait(_FrameIndex, _CommandMgr->DirectCommandQueue());

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

	delete _SwapChain;
	_SwapChain = nullptr;
	

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