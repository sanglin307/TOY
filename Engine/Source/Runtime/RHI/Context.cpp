#include "Private.h"

RenderContext* GRenderContext = nullptr;
RenderContext& RenderContext::Instance()
{
	check(GRenderContext);
	return *GRenderContext;
}

RenderContext::RenderContext()
{
	GRenderContext = this;
}

void RenderContext::Prepare(u32 frameIndex)
{
	_DirectCommandList->Prepare(_DirectCommandAllocator[frameIndex]);
}

void RenderContext::End(Texture2DResource* presentResource)
{
	_DirectCommandList->End(presentResource);

	std::vector<CommandList*> commandList;
	commandList.push_back(_DirectCommandList);
	_DirectCommandQueue->Excute(commandList);
}

void RenderContext::SetViewport(const Viewport& viewport)
{
	_DirectCommandList->SetViewport(viewport);
}
void RenderContext::SetScissorRect(u32 left, u32 top, u32 right, u32 bottom)
{
	_DirectCommandList->SetScissorRect(left, top, right, bottom);
}
void RenderContext::SetRenderTargets(std::vector<Texture2DResource*>& rts, Texture2DResource* depthStencil)
{
	_DirectCommandList->SetRenderTargets(rts, depthStencil);
}
void RenderContext::ClearRenderTarget(Texture2DResource* renderTarget, std::array<float, 4>& colors)
{
	_DirectCommandList->ClearRenderTarget(renderTarget, colors);
}
void RenderContext::SetGraphicsRootSignature(RootSignature* signature)
{
	_DirectCommandList->SetGraphicsRootSignature(signature);
}

void RenderContext::CopyResource(RenderResource* dstRes, RenderResource* srcRes)
{
	_DirectCommandList->CopyResource(dstRes, srcRes);
}