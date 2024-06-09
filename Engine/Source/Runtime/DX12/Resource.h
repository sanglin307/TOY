#pragma once

struct DX12FormatInfo
{
	DXGI_FORMAT   DXFormat;
};


class DX12Swapchain : public Swapchain
{
	friend class DX12Device;
public:
	DX12Swapchain(const Swapchain::Desc& info, std::vector<RenderTexture*>& renderTargets, ComPtr<IDXGISwapChain3> handle)
		:Swapchain(info)
	{
		_RenderTargets = renderTargets;
		_Handle = handle;
	}
	virtual ~DX12Swapchain();

	virtual u32 GetCurrentFrameIndex() override { return _Handle->GetCurrentBackBufferIndex(); }
	virtual RenderTexture* GetCurrentBackBuffer() override
	{
		return _RenderTargets[GetCurrentFrameIndex()];
	}

	virtual void Present(bool vSync) override;

private:
	std::vector<RenderTexture*> _RenderTargets;
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

class DX12RenderBuffer : public RenderBuffer
{
	friend class DX12Device;
public:
	virtual ~DX12RenderBuffer() { _Handle.Reset(); }
	virtual std::any Handle() override { return _Handle.Get(); }
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView();
private:
	DX12RenderBuffer(const RenderBuffer::Desc& desc,ResourceState state, ComPtr<ID3D12Resource> handle)
	{
		State = state;
		_Desc = desc;
		_Handle = handle;
	}

private: 
	ComPtr<ID3D12Resource> _Handle;
};

class DX12RenderTexture : public RenderTexture
{
	friend class DX12Device;
public:
	virtual ~DX12RenderTexture() { _Handle.Reset(); }
	virtual std::any Handle() override { return _Handle.Get(); } 
	D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView() 
	{
		check(_Desc.Usage & (u32)ResourceUsage::RenderTarget);
		return _RenderTargetView;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView()
	{
		check(_Desc.Usage & (u32)ResourceUsage::DepthStencil);
		return _DepthStencilView;
	}

	void SetRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE handle)
	{
		_Desc.Usage |= (u32)ResourceUsage::RenderTarget;
		_RenderTargetView = handle;
	}

	void SetDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE handle)
	{
		_Desc.Usage |= (u32)ResourceUsage::DepthStencil;
		_DepthStencilView = handle;
	}

private:

	DX12RenderTexture(const Desc& desc, ComPtr<ID3D12Resource> handle)
	{
		_Desc = desc;
		_Handle = handle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE _RenderTargetView;
	D3D12_CPU_DESCRIPTOR_HANDLE _DepthStencilView;
	ComPtr<ID3D12Resource> _Handle;
};

