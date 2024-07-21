#pragma once

class DX12CommandQueue : public CommandQueue
{
	friend class DX12Device;
public:
	virtual std::any Handle() { return _Handle.Get(); };
	virtual ~DX12CommandQueue() { _Handle.Reset(); }

	virtual void Excute(u32 ctxNum, RenderContext** ctx) override;
	virtual void Signal(Fence* fence, u64 value) override;
	virtual void Wait(Fence* fence, u64 value) override;
private:
	DX12CommandQueue(CommandType type, ComPtr<ID3D12CommandQueue> handle)
		:_Handle(handle)
	{
		_Type = type;
	}

	ComPtr<ID3D12CommandQueue> _Handle;
};

class DX12CommandAllocator : public CommandAllocator
{
	friend class DX12Device;
public:
	virtual ~DX12CommandAllocator() { _Handle.Reset(); }
	virtual std::any Handle() { return _Handle.Get(); }
private:
	DX12CommandAllocator(CommandType type, ComPtr<ID3D12CommandAllocator> handle)
		:_Handle(handle)
	{
		_Type = type;
	}

	ComPtr<ID3D12CommandAllocator> _Handle;
	
};

class DX12CommandList : public RenderContext
{
	friend class DX12Device;

public:
	virtual ~DX12CommandList() { _Handle.Reset(); }
	virtual std::any Handle() { return _Handle.Get(); }

	virtual void Close(RenderTexture* presentResource) override;
	virtual void Close() override;

	virtual void SetRenderPipeline(RenderPipeline* pipeline) override;
	virtual void SetViewport(u32 x, u32 y, u32 width, u32 height, float minDepth = 0.f, float maxDepth = 1.f) override;
	virtual void SetScissorRect(u32 left, u32 top, u32 right, u32 bottom) override;
	virtual void SetRenderTargets(u32 rtNum, RenderTexture** rts, RenderTargetColorFlags colorFlags, RenderTexture* depthStencil, RenderTargetDepthStencilFlags dsFlags) override;
	virtual void ClearRenderTarget(RenderTexture* renderTarget, const Vector4f& colors) override;
	virtual void ClearDepthStencil(RenderTexture* depthTarget, DepthStentilClearFlag flag, float depth, u8 stencil) override;
	virtual void ClearUnorderedAccessView(RenderResource* uavRes, const Vector4f& values) override;
	virtual void ClearUnorderedAccessView(RenderResource* uavRes, const Vector4u& values) override;
	virtual void CopyResource(RenderResource* dstRes, RenderResource* srcRes) override;

	void TransitionStencilState(ResourceState destState, RenderResource* res);
	void TransitionState(ResourceState destState, RenderResource* res);
	void TransitionState(D3D12_RESOURCE_STATES destState, D3D12_RESOURCE_STATES srcState, ID3D12Resource* resource, u32 subResource = 0);
	void TransitionState(ResourceState destState, RenderResource** resources, u32 number);

	virtual void SetRootDescriptorParameter(const ShaderParameter* param, PipelineType type) override;
	virtual void SetRootDescriptorTableParameter(const std::vector<ShaderParameter*>& params, PipelineType type) override;
	virtual void DrawInstanced(u32 vertexCount, u32 instanceCount = 1, u32 vertexOffset = 0, u32 instanceOffset = 0) override;
	virtual void DrawIndexedInstanced(u32 indexCount, u32 instanceCount = 1, u32 vertexOffset = 0, u32 instanceOffset = 0) override;
	virtual void Dispatch(u32 ThreadGroupCountX, u32 ThreadGroupCountY, u32 ThreadGroupCountZ) override;

	virtual void SetPrimitiveTopology(const PrimitiveTopology topology) override;
	virtual void SetRootSignature(const RootSignature* rootsig, PipelineType type) override;
	virtual void SetDescriptorHeap() override;

	virtual void SetVertexBuffers(u32 vbNum, RenderBuffer** vbs) override;
	virtual void SetIndexBuffer(RenderBuffer* indexBuffer) override;

	void UpdateSubresource(RenderTexture* destResource, RenderBuffer* tempResource, u64 tempResOffset, u32 firstSubresource, u32 numSubresources, const D3D12_SUBRESOURCE_DATA* srcData);
private:
	virtual void Reset() override;

	DX12CommandList(CommandAllocator* allocator, CommandType type, ContextManager* manager,DX12Device* device, ComPtr<ID3D12GraphicsCommandList> handle)
		:RenderContext(type,allocator,manager)
	{
		_Device = device;
		_Handle = handle;
	}
	
	DX12Device* _Device;
	ComPtr<ID3D12GraphicsCommandList> _Handle;
};

class DX12Fence : public Fence
{
	friend class DX12Device;
public:
	virtual ~DX12Fence()
	{
		_Handle.Reset();
	}

	virtual std::any Handle() override { return _Handle.Get(); }
	virtual void CpuWait(u64 fenceValue) override;
	virtual u64 GetCompletedValue() { return _Handle->GetCompletedValue(); }

private:
	DX12Fence(HANDLE fenceEvent,ComPtr<ID3D12Fence> handle)
		:_FenceEvent(fenceEvent), _Handle(handle)
	{
	}

	HANDLE _FenceEvent;
	ComPtr<ID3D12Fence> _Handle;
};