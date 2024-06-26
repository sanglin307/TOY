#include "Private.h"

SceneRenderer::~SceneRenderer()
{
	for (u32 p = 0; p < (u32)RenderPassType::Max; p++)
	{
		delete _Passes[p];
	}
}
SceneRenderer::SceneRenderer(RenderDevice* device)
{
	_Device = device;
	_RenderConfig = GameEngine::Instance().GetRenderConfig();

	_Passes[(u32)RenderPassType::Test] = new RenderPassTest;

	for (u32 p = 0; p < (u32)RenderPassType::Max; p++)
	{
		_Passes[p]->Init(device);
	}
}

void SceneRenderer::BindScene(RenderScene* scene)
{
	_Scene = scene;
}

void SceneRenderer::AddCluster(RenderCluster* cluster)
{
	for (u32 p = 0; p < (u32)RenderPassType::Max; p++)
	{
		_Passes[p]->AddCluster(cluster);
	}
}

void SceneRenderer::RemoveCluster(RenderCluster* cluster)
{
	for (u32 p = 0; p < (u32)RenderPassType::Max; p++)
	{
		_Passes[p]->RemoveCluster(cluster);
	}
}

void SceneRenderer::Render(Swapchain* sc)
{
	RenderContext* ctx = _Device->BeginFrame(sc);

	//resource init and copy op
	_Device->CommitCopyCommand();
	
	ctx->SetViewport(0, 0, _RenderConfig.FrameWidth, _RenderConfig.FrameHeight);
	ctx->SetScissorRect(0, 0, _RenderConfig.FrameWidth, _RenderConfig.FrameHeight);

	RenderTexture* rts[] = { sc->GetCurrentBackBuffer() };
	ctx->SetRenderTargets(1, rts, nullptr);

	const f32 colors[] = { 0.f, 0.f, 0.f, 1.0f };
	ctx->ClearRenderTarget(rts[0], colors);

	for (u32 p = 0; p < (u32)RenderPassType::Max; p++)
	{
		_Passes[p]->Render(_Device, ctx);
	}


	_Device->GpuWaitCopyFinish();
	_Device->EndFrame(ctx, sc);
	_Device->CleanDelayDeleteResource();
}

 