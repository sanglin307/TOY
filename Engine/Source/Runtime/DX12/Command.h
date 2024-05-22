#pragma once

class DX12CommandQueue : public CommandQueue
{
	friend class DX12Device;
public:
	virtual std::any Handle() { return _Handle.Get(); };
	virtual ~DX12CommandQueue() { _Handle.Reset(); }

	virtual void Excute(u32 ctxNum, RenderContext** ctx) override;
	virtual void Signal(Fence* fence, u64 value) override;
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

	virtual void Reset() override;
	virtual void Close(RenderTexture* presentResource) override;
	virtual void SetViewport(u32 x, u32 y, u32 width, u32 height, f32 minDepth = 0.f, f32 maxDepth = 1.f) override;
	virtual void SetScissorRect(u32 left, u32 top, u32 right, u32 bottom) override;
	virtual void SetRenderTargets(u32 rtNum, RenderTexture** rts, RenderTexture* depthStencil) override;
	virtual void ClearRenderTarget(RenderTexture* renderTarget, const f32* colors) override;
	virtual void SetGraphicsRootSignature(RootSignature* signature) override;
	virtual void CopyResource(RenderResource* dstRes, RenderResource* srcRes) override;

	void TransitionState(D3D12_RESOURCE_STATES destState, D3D12_RESOURCE_STATES srcState, ID3D12Resource* resource);
	void TransitionState(D3D12_RESOURCE_STATES* destStates, D3D12_RESOURCE_STATES* srcState, const ID3D12Resource** resource, u32 number);
	void TransitionState(D3D12_RESOURCE_STATES destState, DX12RenderTexture* texture);
	void TransitionState(D3D12_RESOURCE_STATES destState, DX12RenderTexture** texture, u32 number);
	void TransitionState(D3D12_RESOURCE_STATES destState, DX12RenderBuffer* buffer);

private:
	DX12CommandList(CommandAllocator* allocator, CommandType type, ComPtr<ID3D12GraphicsCommandList> handle)
		:RenderContext(type,allocator)
	{
		_Handle = handle;
	}

	

	
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