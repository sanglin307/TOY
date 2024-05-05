#pragma once

class RenderContext
{
public:
	static RenderContext& Instance();
	RenderContext();
	virtual ~RenderContext();

	void Prepare(u32 frameIndex);
	void End(Texture2DResource* presentResource);

	void SetViewport(const Viewport& viewport);
	void SetScissorRect(u32 left, u32 top, u32 right, u32 bottom);
	void SetRenderTargets(std::vector<Texture2DResource*>& rts, Texture2DResource* depthStencil);
	void ClearRenderTarget(Texture2DResource* renderTarget, std::array<float, 4>& colors);
	void SetGraphicsRootSignature(RootSignature* signature);
	void CopyResource(RenderResource* dstRes, RenderResource* srcRes);

	CommandQueue* DirectCommandQueue() { return _DirectCommandQueue; }
	CommandList* GetCopyCommandList() { return _DirectCommandList; }
	CommandList* GetDirectCommandList() { return _DirectCommandList; }

};