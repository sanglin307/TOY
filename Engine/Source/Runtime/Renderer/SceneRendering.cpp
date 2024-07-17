#include "Private.h"

SceneRenderer::~SceneRenderer()
{
	delete _ViewUniformBuffer;

	for (u32 p = 0; p < (u32)RenderPassType::Max; p++)
	{
		delete _Passes[p];
	}
}

void SceneRenderer::InitSceneTextures()
{
	if (_SceneTextures.SceneColor)
	{
		delete _SceneTextures.SceneColor;
	}

	if (_SceneTextures.SceneDepth)
	{
		delete _SceneTextures.SceneDepth;
	}

	const RenderConfig& config = GameEngine::Instance().GetRenderConfig();
	RenderTexture::Desc cds = {
		.Width = config.FrameWidth,
		.Height = config.FrameHeight,
		.DepthOrArraySize = 1,
		.MipLevels = 1,
		.Format = PixelFormat::R16G16B16A16_FLOAT,
		.Usage = (u32)ResourceUsage::RenderTarget | (u32)ResourceUsage::ShaderResource,
		.Dimension = ResourceDimension::Texture2D
	};
	_SceneTextures.SceneColor = _Device->CreateTexture("SceneColor", cds);

	RenderTexture::Desc dds = {
		.Width = config.FrameWidth,
		.Height = config.FrameHeight,
		.DepthOrArraySize = 1,
		.MipLevels = 1,
		.Format = PixelFormat::D32_FLOAT_S8X24_UINT,
		.Usage = (u32)ResourceUsage::DepthStencil | (u32)ResourceUsage::ShaderResource,
		.Dimension = ResourceDimension::Texture2D
	};
	_SceneTextures.SceneDepth = _Device->CreateTexture("SceneDepth", dds);


	RenderTexture::Desc cods = {
		.Width = config.FrameWidth,
		.Height = config.FrameHeight,
		.DepthOrArraySize = 1,
		.MipLevels = 1,
		.Format = PixelFormat::R8G8B8A8_UNORM,
		.Usage = (u32)ResourceUsage::RenderTarget | (u32)ResourceUsage::ShaderResource | (u32)ResourceUsage::UnorderedAccess,
		.Dimension = ResourceDimension::Texture2D
	};
	_SceneTextures.ColorOutput = _Device->CreateTexture("ColorOutput", cods);

}

void SceneRenderer::InitRenderPass()
{
	_Passes[(u32)RenderPassType::Forward] = new RenderPassForward;

	for (u32 p = 0; p < (u32)RenderPassType::Max; p++)
	{
		_Passes[p]->Init(_Device, this);
	}
}

SceneRenderer::SceneRenderer(RenderDevice* device)
{
	_Device = device;

	RenderBuffer::Desc udesc = {
		.Size = sizeof(ViewInfo),
		.Usage = (u32)ResourceUsage::UniformBuffer,
		.CpuAccess = CpuAccessFlags::Write,
		.Alignment = true
	};
	_ViewUniformBuffer = device->CreateBuffer("ViewInfo", udesc);

	InitSceneTextures();
	InitRenderPass();
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

void SceneRenderer::Render(ViewInfo& view, Swapchain* sc)
{
	RenderContext* ctx = _Device->BeginFrame(sc);
	
	_ViewUniformBuffer->UploadData((u8*)&view, sizeof(ViewInfo));

	ctx->SetDescriptorHeap();
	_Passes[(u32)RenderPassType::Forward]->Render(view, sc, ctx);
	ctx->CopyResource(sc->GetCurrentBackBuffer(), _SceneTextures.ColorOutput);

	_Device->EndFrame(ctx, sc);
}

 