#pragma once

struct DX12FormatInfo
{
	DXGI_FORMAT   DXFormat;
};


class DX12Swapchain : public Swapchain
{
	friend class DX12Device;
public:
	DX12Swapchain(const Swapchain::Desc& info, DescriptorAllocation& rvtAlloc,std::vector<RenderTexture*>& renderTargets, ComPtr<IDXGISwapChain3> handle)
		:Swapchain(info)
	{
		_RenderTargetsHeapAlloc = rvtAlloc;
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
	DescriptorAllocation _RenderTargetsHeapAlloc;
	std::vector<RenderTexture*> _RenderTargets;
	ComPtr<IDXGISwapChain3> _Handle;
};

class DX12DescriptorHeap : public DescriptorHeap
{
	friend class DX12Device;
public:
	virtual ~DX12DescriptorHeap() { _Handle.Reset(); }
	virtual std::any Handle() override;
	virtual std::any CPUHandle(DescriptorAllocation& pos) override;
	virtual std::any GPUHandle(DescriptorAllocation& pos) override;
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
	virtual ~DX12RenderBuffer() 
	{
		if (_UniformDataMapPointer)
		{
			_Handle->Unmap(0, nullptr);
		}

		if (_Desc.Usage & (u32)ResourceUsage::UniformBuffer && _CBVDescriptor.Count > 0)
		{
			Device->GetDescriptorHeap(DescriptorType::CBV_SRV_UAV)->Free(_CBVDescriptor);
		}

		_Handle.Reset(); 
	}
	virtual std::any Handle() override { return _Handle.Get(); }
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView();
	DescriptorAllocation GetCBVDescriptor()
	{
		return _CBVDescriptor;
	}

	virtual void UploadData(u8* data, size_t size) override;

private:
	DX12RenderBuffer(RenderDevice* device, const RenderBuffer::Desc& desc,ResourceState state, ComPtr<ID3D12Resource> handle)
	{
		Device = device;
		State = state;
		_Desc = desc;
		_Handle = handle;
	}

private: 
	DescriptorAllocation _CBVDescriptor;
	D3D12_CPU_DESCRIPTOR_HANDLE _ConstBufferView;
	u8* _UniformDataMapPointer;
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

	void SetRenderTargetView(DescriptorAllocation rvtDescriptor, D3D12_CPU_DESCRIPTOR_HANDLE handle)
	{
		_Desc.Usage |= (u32)ResourceUsage::RenderTarget;
		_RenderTargetView = handle;
		_RVTDescriptor = rvtDescriptor;
	}

	void SetDepthStencilView(DescriptorAllocation dsvDescriptor, D3D12_CPU_DESCRIPTOR_HANDLE handle)
	{
		_Desc.Usage |= (u32)ResourceUsage::DepthStencil;
		_DepthStencilView = handle;
		_DSVDescriptor = dsvDescriptor;
	}

private:

	DX12RenderTexture(RenderDevice* device, const Desc& desc, ComPtr<ID3D12Resource> handle)
	{
		Device = device;
		_Desc = desc;
		_Handle = handle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE _RenderTargetView;
	DescriptorAllocation _RVTDescriptor;
	D3D12_CPU_DESCRIPTOR_HANDLE _DepthStencilView;
	DescriptorAllocation _DSVDescriptor;
	ComPtr<ID3D12Resource> _Handle;
};

