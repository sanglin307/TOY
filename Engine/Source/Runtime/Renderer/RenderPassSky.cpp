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

	ProceduralSky skyCB = {};
	RenderBuffer::Desc rd = {
		.Size = sizeof(ProceduralSky),
		.Stride = sizeof(ProceduralSky),
		.Usage = (u32)ResourceUsage::UniformBuffer,
		.CpuAccess = CpuAccessFlags::Write,
		.Alignment = true
	};
	_SkyCB = _Device->CreateBuffer("SkyCB", rd);

	_SkyPso->BindParameter("ViewCB", _Renderer->GetViewUniformBuffer());
	_SkyPso->BindParameter("SkyCB", _SkyCB);
}



void RenderPassSky::Render(ViewInfo& view, Swapchain* sc, RenderContext* ctx)
{
	const SceneTextures& sceneTextures = _Renderer->GetSceneTextures();

	{
		RenderMarker marker(ctx, float3(0.18, 0.18, 0.18), "SkyPass");
		ProceduralSky skyCB = {
			.SunDirection = float3(1.f,0.05f,0.f),
			.Rayleigh = 2.f,
			.Turbidity = 10.f,
			.MieCoefficient = 0.005f,
			.Luminance = 1.f,
			.MieDirectionalG = 0.8f
		};
		_SkyCB->UploadData((u8*)&skyCB, sizeof(ProceduralSky));

		_SkyPso->CommitParameter(ctx);

		RenderTexture* rts[] = { sceneTextures.SceneColor };
		ctx->SetRenderTargets(1, rts, RenderTargetColorFlags::None, sceneTextures.SceneDepth, RenderTargetDepthStencilFlags::None);
 
		ctx->SetRootSignature(_SkyPso->GetRootSignature(), PipelineType::Graphic);
		ctx->SetPrimitiveTopology(_SkyPso->Info.Topology);
		ctx->SetRenderPipeline(_SkyPso);
		ctx->DrawInstanced(3);
	}

}