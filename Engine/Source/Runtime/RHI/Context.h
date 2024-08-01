#pragma once

enum class CommandType
{
	Direct = 0,
	Bundle,
	Compute,
	Copy
};

enum class ContextState
{
	Open = 0,
	Close
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

	virtual void Close(RenderTexture* presentResource) = 0;
	virtual void Close() = 0;

	virtual void SetRenderPipeline(RenderPipeline* pipeline) = 0;
	virtual void SetViewport(u32 x, u32 y, u32 width, u32 height, float minDepth = 0.f, float maxDepth = 1.f) = 0;
	virtual void SetScissorRect(u32 left, u32 top, u32 right, u32 bottom) = 0;
	virtual void SetRenderTargets(u32 rtNum, RenderTexture** rts, RenderTargetColorFlags colorFlags, RenderTexture* depthStencil, RenderTargetDepthStencilFlags dsFlags) = 0;
	virtual void ClearRenderTarget(RenderTexture* renderTarget, const Vector4f& colors) = 0;
	virtual void ClearDepthStencil(RenderTexture* depthTarget, DepthStentilClearFlag flag, float depth, u8 stencil) = 0;
	virtual void ClearUnorderedAccessView(RenderResource* uavRes, const Vector4f& value) = 0;
	virtual void ClearUnorderedAccessView(RenderResource* uavRes, const Vector4u& values) = 0;
	virtual void CopyResource(RenderResource* dstRes, RenderResource* srcRes) = 0;
	virtual void SetRootDescriptorParameter(const ShaderParameter* param, PipelineType type) = 0;
	virtual void SetRootDescriptorTableParameter(const std::vector<ShaderParameter*>& params, PipelineType type) = 0;
	virtual void DrawInstanced(u32 vertexCount, u32 instanceCount = 1, u32 vertexOffset = 0, u32 instanceOffset = 0) = 0;
	virtual void DrawIndexedInstanced(u32 indexCount, u32 instanceCount = 1, u32 vertexOffset = 0, u32 instanceOffset = 0) = 0;
	virtual void SetPrimitiveTopology(const PrimitiveTopology topology) = 0;
	virtual void SetRootSignature(const RootSignature* rootsig, PipelineType type) = 0;
	virtual void SetDescriptorHeap() = 0;

	virtual void Dispatch(u32 ThreadGroupCountX, u32 ThreadGroupCountY, u32 ThreadGroupCountZ) = 0;

	virtual void SetVertexBuffers(u32 vbNum, RenderBuffer** vbs) = 0;
	virtual void SetIndexBuffer(RenderBuffer* indexBuffer) = 0;

	virtual void BeginRenderMarker(const float3& color, const std::string& name) {};
	virtual void EndRenderMarker() {};
	virtual void SetRenderMarker(const float3& color, const std::string& name) {};

	virtual void ImGuiRenderDrawData() = 0;
	 
	RenderContext* Open();

protected:

	virtual void Reset() = 0;

	CommandType _Type;
	CommandAllocator* _Allocator;
	ContextManager* _Manager;
	ContextState _State;
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
	RHI_API ContextManager(RenderDevice* device, u32 contextCount);
	RHI_API ~ContextManager();

	RHI_API void WaitGPUIdle();
	RHI_API RenderContext* GetDirectContext(u32 index);
	RHI_API CommandQueue* GetDirectQueue() { return _DirectCommandQueue; }
	RHI_API void SwitchToNextFrame(u32 lastFrameIndex, u32 nextFrameIndex);

	RHI_API RenderContext* GetCopyContext(u64& copyFenceValue);
	RHI_API void CommitCopyCommand();
	RHI_API void WaitCopyFinish();

	RHI_API u64 GetMaxFrameFenceValue();
	u64 GetFrameFenceCompletedValue();
	u64 GetCopyQueueFenceCompletedValue();
private:
	u32 _DirectContextNumber;
	CommandQueue* _DirectCommandQueue;
	std::vector<CommandAllocator*> _DirectCommandAllocators;
	std::vector<RenderContext*> _DirectContexts;
	Fence* _FrameFence;
	std::vector<u64> _FenceValues;

	CommandQueue* _ComputeCommandQueue;
	CommandAllocator* _ComputeCommandAllocator;
	RenderContext* _ComputeContext;

	CommandQueue* _CopyCommandQueue;
	CommandAllocator* _CopyCommandAllocator;
	RenderContext* _CopyContext;
	u64 _PrepareCopyFenceValue = 0;
	u64 _ComittedCopyFenceValue = 0;
	Fence* _CopyQueueFence;

	RenderDevice* _Device;
	
};

class RenderMarker
{
public:
	RHI_API RenderMarker(RenderContext* ctx, const float3& color, const std::string& name);
	RHI_API ~RenderMarker();

private:
	RenderContext* _Context;
};