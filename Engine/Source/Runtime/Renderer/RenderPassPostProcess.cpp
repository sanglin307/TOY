#include "Private.h"

RenderPassTonemap::RenderPassTonemap(RenderDevice* device, SceneRenderer* renderer)
	:RenderPass(RenderPassType::Tonemap,device,renderer)
{
	ComputePipeline::Desc cd = {
		.CS = {
		   .Profile = ShaderProfile::Compute,
		   .Path = "Tonemap.hlsl",
		   .Entry = "CSMain"
		}
	};
	_TonemapPso = static_cast<ComputePipeline*>(device->CreateComputePipeline("TonemapCS", cd));
}

 

void RenderPassTonemap::Render(ViewInfo& view, Swapchain* sc, RenderContext* ctx)
{
	const SceneTextures& sceneTextures = _Renderer->GetSceneTextures();

	{
		RenderMarker marker(ctx, float3(0.3, 0.3, 0.3), "Tonemapping");
		ctx->SetRootSignature(_TonemapPso->GetRootSignature(), PipelineType::Compute);
		ctx->SetRenderPipeline(_TonemapPso);
		u32 voff = _Device->GetDynamicRingBuffer()->AllocateConstBuffer((u8*)&view, sizeof(ViewInfo));
		_TonemapPso->BindParameter("ViewCB", _Device->GetDynamicRingBuffer()->GetResource(), voff);
		_TonemapPso->BindParameter("SceneColor", sceneTextures.SceneColor);
		_TonemapPso->BindParameter("ColorUAV", sceneTextures.ColorOutput);
		_TonemapPso->CommitParameter(ctx);
		ctx->Dispatch(DivideRoundup(view.ViewportSize.x, 16), DivideRoundup(view.ViewportSize.y, 16), 1);
	}

}

 

 