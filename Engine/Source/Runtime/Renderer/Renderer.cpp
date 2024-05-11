#include "Private.h"

IMPLEMENT_MODULE(RendererModule)

IRHIModule& GetRHI(RenderAPI api)
{
	IRHIModule* module = nullptr;
	if (api == RenderAPI::DX12)
	{
		module = static_cast<IRHIModule*>(ModuleManager::LoadModule("DX12"));
	}

	check(module);

	return *module;
}

void RendererModule::Init()
{
	_Scene = new RenderScene;
	_Scene->Init();

	_RenderConfig = GameEngine::Instance().GetRenderConfig();

	_Device = GetRHI(_RenderConfig.API).GetDevice();
}
 
void RendererModule::Render(RHIViewport* viewport)
{
	RenderContext* ctx = _Device->BeginFrame(viewport);

	ctx->SetViewport(0, 0, _RenderConfig.FrameWidth, _RenderConfig.FrameHeight);
	ctx->SetScissorRect(0, 0, _RenderConfig.FrameWidth, _RenderConfig.FrameHeight);

	Texture2DResource* rts[] = { viewport->GetCurrentBackBuffer() };
	ctx->SetRenderTargets(1, rts, nullptr);

	const f32 colors[] = {0.0f, 1.f, 0.4f, 1.0f};
	ctx->ClearRenderTarget(rts[0], colors);

	_Device->EndFrame(ctx,viewport);
}

void RendererModule::Destroy()
{
	_Scene->Destroy();
	delete _Scene;
	_Scene = nullptr;

	RootSignatureManager::Destroy();
	PipelineManager::Destroy();
	ShaderManager::Destroy();
 

	
}