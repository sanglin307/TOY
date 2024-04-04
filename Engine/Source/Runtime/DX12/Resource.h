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
private:
	DX12SwapChain(const SwapChain::Config& config,std::vector<RenderResource*>& renderTargets,
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& renderTargetViews, ComPtr<IDXGISwapChain3> handle)
	{
		_Config = config;
		_RenderTargets = renderTargets;
		_RVTDescriptors = renderTargetViews;
		_Handle = handle;
	}

	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> _RVTDescriptors;
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
	DX12BufferResource(ComPtr<ID3D12Resource> handle)
	{
		_Handle = handle;
	}

private:
	ComPtr<ID3D12Resource> _Handle;
};

class DX12Resource : public RenderResource
{
	friend class DX12Device;
public:
	virtual ~DX12Resource() { _Handle.Reset(); }
private:

	DX12Resource(ComPtr<ID3D12Resource> handle)
	{
		_Handle = handle;
	}

	ComPtr<ID3D12Resource> _Handle;
};

