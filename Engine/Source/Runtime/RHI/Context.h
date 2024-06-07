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
class ContextManager;

class RenderContext
{
public:
	RHI_API RenderContext(CommandType type, CommandAllocator* allocator, ContextManager* manager);
	RHI_API virtual ~RenderContext() {};
	virtual std::any Handle() { return nullptr; }

	virtual void Reset() = 0;
	virtual void Close(RenderTexture* presentResource) = 0;
	virtual void Close() = 0;

	virtual void SetViewport(u32 x, u32 y, u32 width, u32 height, f32 minDepth = 0.f, f32 maxDepth = 1.f) = 0;
	virtual void SetScissorRect(u32 left, u32 top, u32 right, u32 bottom) = 0;
	virtual void SetRenderTargets(u32 rtNum, RenderTexture** rts, RenderTexture* depthStencil) = 0;
	virtual void ClearRenderTarget(RenderTexture* renderTarget, const f32* colors) = 0;
	virtual void CopyResource(RenderResource* dstRes, RenderResource* srcRes, ResourceState dstResAfterState = ResourceState::Reserve) = 0;
	virtual void TransitionState(ResourceState destState, RenderResource* buffer) = 0;

protected:
	CommandType _Type;
	CommandAllocator* _Allocator;
	ContextManager* _Manager;
};

class CommandQueue
{
public:
	virtual ~CommandQueue() {};
	virtual std::any Handle() { return nullptr; }
	virtual void Signal(Fence* fence, u64 value) = 0;
	virtual void Excute(u32 ctxNum, RenderContext** ctx) = 0;
	virtual void Wait(Fence* fence, u64 value) = 0;
protected:
	CommandType _Type;
};

class ContextManager
{
public:
	RHI_API ContextManager(RenderDevice* device);
	RHI_API ~ContextManager();

	static constexpr u32 CommandAllocatorNumber = 3;

	RHI_API void WaitGPUIdle();
	RenderContext* GetDirectContext(u32 index) { return _DirectContexts[index]; }
	CommandQueue* GetDirectQueue() { return _DirectCommandQueue; }
	RHI_API void SwitchToNextFrame(u32 lastFrameIndex, u32 nextFrameIndex);

	RenderContext* GetCopyContext() {  return _CopyContext; }
	RHI_API void AddCopyNum();
	RHI_API void CommitCopyCommand();
	RHI_API void GpuWaitCopyFinish();

private:
	CommandQueue* _DirectCommandQueue;
	std::vector<CommandAllocator*> _DirectCommandAllocators;
	std::vector<RenderContext*> _DirectContexts;
	Fence* _FrameFence;

	CommandQueue* _ComputeCommandQueue;
	CommandAllocator* _ComputeCommandAllocator;
	RenderContext* _ComputeContext;

	CommandQueue* _CopyCommandQueue;
	CommandAllocator* _CopyCommandAllocator;
	RenderContext* _CopyContext;
	u64 _CopyQueueFenceValue = 0;
	Fence* _CopyQueueFence;
	bool _ContainCopyOp = false;

	RenderDevice* _Device;
	u64 _FenceValues[CommandAllocatorNumber];
};