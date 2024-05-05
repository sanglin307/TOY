#pragma once

enum class CommandType
{
	Direct = 0,
	Bundle,
	Compute,
	Copy
};

class RenderDevice;


class CommandAllocator
{
public:
	virtual ~CommandAllocator() {};
	virtual std::any Handle() { return nullptr; }

protected:
	CommandType _Type;

};

class CommandList
{
public:
	virtual ~CommandList() {};
	virtual std::any Handle() { return nullptr; }

	virtual void Prepare(CommandAllocator* allocator) = 0;
	virtual void End(Texture2DResource* presentResource) = 0;

	virtual void SetViewport(const Viewport& viewport) = 0;
	virtual void SetScissorRect(u32 left, u32 top, u32 right, u32 bottom) = 0;
	virtual void SetRenderTargets(std::vector<Texture2DResource*>& rts, Texture2DResource* depthStencil) = 0;
	virtual void ClearRenderTarget(Texture2DResource* renderTarget, std::array<float, 4>& colors) = 0;
	virtual void SetGraphicsRootSignature(RootSignature* signature) = 0;
	virtual void CopyResource(RenderResource* dstRes, RenderResource* srcRes) = 0;

protected:
	CommandType _Type;
};

class CommandQueue
{
public:
	virtual ~CommandQueue() {};
	virtual std::any Handle() { return nullptr; }
	virtual void Excute(std::vector<CommandList*> commands) = 0;
protected:
	CommandType _Type;
};


class Fence
{
public:
	virtual ~Fence() {};
	virtual void Wait(u32 frameIndex, CommandQueue* commandQueue) = 0;
	virtual void Advance(u32 frameIndex, u32 nextFrameIndex, CommandQueue* commandQueue) = 0;
protected:
	u64* _FenceValues;
};

