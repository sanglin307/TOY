#pragma once

class DX12CommandQueue : public CommandQueue
{
	friend class DX12Device;
public:
	virtual std::any Handle() { return _Handle.Get(); };
	virtual ~DX12CommandQueue() { _Handle.Reset(); }
	virtual void Excute(std::vector<CommandList*> commands) override;
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

class DX12CommandList : public CommandList
{
	friend class DX12Device;

public:
	virtual ~DX12CommandList() { _Handle.Reset(); }
	virtual std::any Handle() { return _Handle.Get(); }

	virtual void Prepare(CommandAllocator* allocator) override;
	virtual void End(Texture2DResource* presentResource) override;
	virtual void SetViewport(const Viewport& viewport) override;
	virtual void SetScissorRect(u32 left, u32 top, u32 right, u32 bottom) override;
	virtual void SetRenderTargets(std::vector<Texture2DResource*>& rts, Texture2DResource* depthStencil) override;
	virtual void ClearRenderTarget(Texture2DResource* renderTarget, std::array<float, 4>& colors) override;
	virtual void SetGraphicsRootSignature(RootSignature* signature) override;
	virtual void CopyResource(RenderResource* dstRes, RenderResource* srcRes) override;


private:
	DX12CommandList(CommandType type, ComPtr<ID3D12GraphicsCommandList> handle)
		:_Handle(handle)
	{
		_Type = type;
	}
	
	ComPtr<ID3D12GraphicsCommandList> _Handle;
};

class DX12Fence : public Fence
{
	friend class DX12Device;
public:
	virtual ~DX12Fence()
	{
		delete[] _FenceValues;
		_Handle.Reset();
	}

	virtual void Wait(u32 frameIndex, CommandQueue* commandQueue) override;
	virtual void Advance(u32 frameIndex, u32 nextFrameIndex, CommandQueue* commandQueue) override;

private:
	DX12Fence(u64* frameValue, HANDLE fenceEvent,ComPtr<ID3D12Fence> handle)
		:_FenceEvent(fenceEvent), _Handle(handle)
	{
		_FenceValues = frameValue;
	}

	HANDLE _FenceEvent;
	ComPtr<ID3D12Fence> _Handle;
};