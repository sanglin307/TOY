#pragma once

struct DX12FormatInfo
{
	DXGI_FORMAT   DXFormat;
};

class DX12SwapChain : public SwapChain
{
	friend class DX12Device;
public:
	virtual ~DX12SwapChain();
	virtual u32 CurrentFrameIndex() override;
	virtual void Present(bool vSync) override;
private:
	DX12SwapChain(const SwapChain::Config& config,std::vector<Texture2DResource*>& renderTargets, ComPtr<IDXGISwapChain3> handle)
	{
		_Config = config;
		_RenderTargets = renderTargets;
		_Handle = handle;
	}

	ComPtr<IDXGISwapChain3> _Handle;
};

class DX12DescriptorHeap : public DescriptorHeap
{
	friend class DX12Device;
public:
	virtual ~DX12DescriptorHeap() { _Handle.Reset(); }
	virtual std::any Handle() override;
	virtual std::any GetCPUDescriptorHandle(u32 reserve = 0) override;
	virtual std::any GetGPUDescriptorHandle(u32 reserve = 0) override;
private:
	DX12DescriptorHeap(const DescriptorHeap::Config& config, ComPtr<ID3D12DescriptorHeap> handle)
	{
		_Config = config;
		_Handle = handle;
	}
	ComPtr<ID3D12DescriptorHeap> _Handle = nullptr;
};

class DX12BufferResource : public BufferResource
{
	friend class DX12Device;
public:
	virtual ~DX12BufferResource() { _Handle.Reset(); }

private:
	DX12BufferResource(u64 size, u32 usage, u32 stride, bool needCpuAccess, bool needAlignment,ComPtr<ID3D12Resource> handle)
	{
		Size = size;
		Usage = usage;
		Stride = stride;
		NeedCpuAccess = needCpuAccess;
		NeedAlignment = needAlignment;
		_Handle = handle;
	}

private:
	ComPtr<ID3D12Resource> _Handle;
};

class DX12Texture2DResource : public Texture2DResource
{
	friend class DX12Device;
public:
	virtual ~DX12Texture2DResource() { _Handle.Reset(); }
	virtual std::any Handle() override { return _Handle.Get(); } 
	D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView() 
	{
		check(Usage & (u32)ResourceUsage::RenderTarget);
		return _RenderTargetView;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView()
	{
		check(Usage & (u32)ResourceUsage::DepthStencil);
		return _DepthStencilView;
	}

	void SetRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE handle)
	{
		Usage |= (u32)ResourceUsage::RenderTarget;
		_RenderTargetView = handle;
	}

	void SetDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE handle)
	{
		Usage |= (u32)ResourceUsage::DepthStencil;
		_DepthStencilView = handle;
	}

	D3D12_RESOURCE_STATES GetState() const { return _ResourceState; }
	void SetState(D3D12_RESOURCE_STATES state) { _ResourceState = state; }

private:

	DX12Texture2DResource(ComPtr<ID3D12Resource> handle)
	{
		_Handle = handle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE _RenderTargetView;
	D3D12_CPU_DESCRIPTOR_HANDLE _DepthStencilView;
	ComPtr<ID3D12Resource> _Handle;
	D3D12_RESOURCE_STATES _ResourceState;
};

