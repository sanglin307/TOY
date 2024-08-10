#include "Private.h"

RenderPassSky::RenderPassSky(RenderDevice* device, SceneRenderer* renderer)
	:RenderPass(RenderPassType::Sky, device, renderer)
{
	const SceneTextures& sceneTextures = _Renderer->GetSceneTextures();

	GraphicPipeline::Desc desc = {
	.VS = {
	   .Profile = ShaderProfile::Vertex,
	   .Path = "PostProcess.hlsl",
	   .Entry = "VSMain"
	},
	.PS = {
		.Profile = ShaderProfile::Pixel,
		.Path = "ProceduralSky.hlsl",
		.Entry = "PSMain"
	},
	.DepthStencilState = {
		.DepthEnable = true,
		.DepthFunc = ComparisonFunc::Equal
	},
	.RVTFormats = { sceneTextures.SceneColor->GetFormat() },
	.DSVFormat = sceneTextures.SceneDepth->GetFormat()
	};

	_SkyPso = static_cast<GraphicPipeline*>(device->CreateGraphicPipeline("ProceduralSky", desc));
}



void RenderPassSky::Render(ViewInfo& view, Swapchain* sc, RenderContext* ctx)
{
	const SceneTextures& sceneTextures = _Renderer->GetSceneTextures();
	ProceduralSky* sky = _Renderer->GetScene()->GetProceduralSkyData();
	if (!sky)  return;

	{
		RenderMarker marker(ctx, float3(0.18, 0.18, 0.18), "SkyPass");

		RenderTexture* rts[] = { sceneTextures.SceneColor };
		ctx->SetRenderTargets(1, rts, RenderTargetColorFlags::None, sceneTextures.SceneDepth, RenderTargetDepthStencilFlags::None);
		ctx->SetRootSignature(_SkyPso->GetRootSignature(), PipelineType::Graphic);
		ctx->SetPrimitiveTopology(_SkyPso->Info.Topology);
		ctx->SetRenderPipeline(_SkyPso);

		u32 voff = _Device->GetDynamicRingBuffer()->AllocateConstBuffer((u8*)&view, sizeof(ViewInfo));
		_SkyPso->BindParameter("ViewCB", _Device->GetDynamicRingBuffer()->GetResource(), voff);

		u32 soff = _Device->GetDynamicRingBuffer()->AllocateConstBuffer((u8*)sky, sizeof(ProceduralSky));
		_SkyPso->BindParameter("SkyCB", _Device->GetDynamicRingBuffer()->GetResource(), soff);

		_SkyPso->CommitParameter(ctx);

		ctx->DrawInstanced(3);
	}

}