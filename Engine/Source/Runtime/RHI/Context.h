#pragma once

enum class CommandType
{
	Direct = 0,
	Bundle,
	Compute,
	Copy
};

class Fence
{
public:
	virtual ~Fence() {};
	virtual std::any Handle() { return nullptr; }
	virtual void CpuWait(u64 fenceValue) = 0;
	virtual u64 GetCompletedValue() = 0;
};

class CommandAllocator
{
public:
	virtual ~CommandAllocator() {};
	virtual std::any Handle() { return nullptr; }

protected:
	CommandType _Type;

};

class RenderDevice;

class RenderContext
{
public:
	RHI_API RenderContext(CommandType type, CommandAllocator* allocator);
	RHI_API virtual ~RenderContext() {};
	virtual std::any Handle() { return nullptr; }

	virtual void Reset() = 0;
	virtual void Close(RenderTexture* presentResource) = 0;

	virtual void SetViewport(u32 x, u32 y, u32 width, u32 height, f32 minDepth = 0.f, f32 maxDepth = 1.f) = 0;
	virtual void SetScissorRect(u32 left, u32 top, u32 right, u32 bottom) = 0;
	virtual void SetRenderTargets(u32 rtNum, RenderTexture** rts, RenderTexture* depthStencil) = 0;
	virtual void ClearRenderTarget(RenderTexture* renderTarget, const f32* colors) = 0;
	virtual void SetGraphicsRootSignature(RootSignature* signature) = 0;
	virtual void CopyResource(RenderResource* dstRes, RenderResource* srcRes) = 0;

protected:
	CommandType _Type;
	CommandAllocator* _Allocator;
};

class CommandQueue
{
public:
	virtual ~CommandQueue() {};
	virtual std::any Handle() { return nullptr; }
	virtual void Signal(Fence* fence, u64 value) = 0;
	virtual void Excute(u32 ctxNum, RenderContext** ctx) = 0;
protected:
	CommandType _Type;
};

class ContextManager
{
public:
	RHI_API ContextManager(RenderDevice* device);
	RHI_API ~ContextManager();

	static constexpr u32 CommandAllocatorNumber = 3;

	CommandAllocator* GetDirectAllocator(u32 index) { return _DirectCommandAllocators[index]; }
	RenderContext* GetDirectContext(u32 index) { return _DirectContexts[index]; }
	CommandQueue* GetDirectQueue() { return _DirectCommandQueue; }
	u64& GetFenceValue(u32 index) { return _FenceValues[index]; }
	u64 GetMaxFenceValue() 
	{
		u64 value = 0;
		for (u32 i = 0; i < CommandAllocatorNumber; i++)
		{
			if (_FenceValues[i] > value)
				value = _FenceValues[i];
		}
		return value;
	}

private:
	CommandQueue* _DirectCommandQueue;
	std::vector<CommandAllocator*> _DirectCommandAllocators;
	std::vector<RenderContext*> _DirectContexts;

	CommandQueue* _ComputeCommandQueue;
	CommandAllocator* _ComputeCommandAllocator;
	RenderContext* _ComputeContext;

	CommandQueue* _CopyCommandQueue;
	CommandAllocator* _CopyCommandAllocator;
	RenderContext* _CopyContext;

	RenderDevice* _Device;
	u64 _FenceValues[CommandAllocatorNumber];
};