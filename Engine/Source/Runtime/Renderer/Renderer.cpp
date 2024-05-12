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

IRenderScene* RendererModule::AllocateScene(GameWorld* world)
{
	RenderScene* scene = new RenderScene(world);
	_Scenes.insert(scene);
	return scene;
}

void RendererModule::RemoveScene(IRenderScene* scene)
{
	_Scenes.erase(scene);
	delete scene;
}

void RendererModule::Init()
{
	_RenderConfig = GameEngine::Instance().GetRenderConfig();

	_Device = GetRHI(_RenderConfig.API).GetDevice();
}
 
void RendererModule::Render(Swapchain* sc)
{
	RenderContext* ctx = _Device->BeginFrame(sc);

	ctx->SetViewport(0, 0, _RenderConfig.FrameWidth, _RenderConfig.FrameHeight);
	ctx->SetScissorRect(0, 0, _RenderConfig.FrameWidth, _RenderConfig.FrameHeight);

	RenderTexture2D* rts[] = { sc->GetCurrentBackBuffer() };
	ctx->SetRenderTargets(1, rts, nullptr);

	const f32 colors[] = {0.0f, 1.f, 0.4f, 1.0f};
	ctx->ClearRenderTarget(rts[0], colors);

	_Device->EndFrame(ctx,sc);
}

void RendererModule::Destroy()
{
	check(_Scenes.empty());

	RootSignatureManager::Destroy();
	PipelineManager::Destroy();
	ShaderManager::Destroy();
 

	
}