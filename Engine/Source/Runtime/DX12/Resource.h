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
	D3D12_CPU_DESCRIPTOR_HANDLE CPUHandle(DescriptorAllocation& pos);
	D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle(DescriptorAllocation& pos);
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

	D3D12_CPU_DESCRIPTOR_HANDLE GetConstBufferViewCPUHandle()
	{
		return _ConstBufferViewCPUHandle;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE GetConstBufferViewGPUHandle()
	{
		return _ConstBufferViewGPUHandle;
	}

	void SetConstBufferView(DescriptorAllocation descriptor, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle)
	{
		_Desc.Usage |= (u32)ResourceUsage::UniformBuffer;
		_CBVDescriptor = descriptor;
		_ConstBufferViewCPUHandle = cpuHandle;
		_ConstBufferViewGPUHandle = gpuHandle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceViewCPUHandle()
	{
		check(_Desc.Usage & (u32)ResourceUsage::ShaderResource);
		return _ShaderResourceViewCPUHandle;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE GetShaderResourceViewGPUHandle()
	{
		check(_Desc.Usage & (u32)ResourceUsage::ShaderResource);
		return _ShaderResourceViewGPUHandle;
	}

	void SetShaderResourcelView(DescriptorAllocation srvDescriptor, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle)
	{
		_Desc.Usage |= (u32)ResourceUsage::ShaderResource;
		_ShaderResourceViewCPUHandle = cpuHandle;
		_ShaderResourceViewGPUHandle = gpuHandle;
		_SRVDescriptor = srvDescriptor;
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
	D3D12_CPU_DESCRIPTOR_HANDLE _ShaderResourceViewCPUHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE _ShaderResourceViewGPUHandle;
	DescriptorAllocation _SRVDescriptor;

	DescriptorAllocation _CBVDescriptor;
	D3D12_CPU_DESCRIPTOR_HANDLE _ConstBufferViewCPUHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE _ConstBufferViewGPUHandle;
	u8* _UniformDataMapPointer;

	ComPtr<ID3D12Resource> _Handle;
};

class DX12RenderTexture : public RenderTexture
{
	friend class DX12Device;
public:
	virtual ~DX12RenderTexture() { _Handle.Reset(); }
	virtual std::any Handle() override { return _Handle.Get(); } 

	D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetViewCPUHandle() 
	{
		check(_Desc.Usage & (u32)ResourceUsage::RenderTarget);
		return _RenderTargetViewCPUHandle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilViewCPUHandle()
	{
		check(_Desc.Usage & (u32)ResourceUsage::DepthStencil);
		return _DepthStencilViewCPUHandle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceViewCPUHandle()
	{
		check(_Desc.Usage & (u32)ResourceUsage::ShaderResource);
		return _ShaderResourceViewCPUHandle;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE GetShaderResourceViewGPUHandle()
	{
		check(_Desc.Usage & (u32)ResourceUsage::ShaderResource);
		return _ShaderResourceViewGPUHandle;
	}

	void SetRenderTargetView(DescriptorAllocation rvtDescriptor, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle)
	{
		_Desc.Usage |= (u32)ResourceUsage::RenderTarget;
		_RenderTargetViewCPUHandle = cpuHandle;
		_RVTDescriptor = rvtDescriptor;
	}

	void SetDepthStencilView(DescriptorAllocation dsvDescriptor, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle)
	{
		_Desc.Usage |= (u32)ResourceUsage::DepthStencil;
		_DepthStencilViewCPUHandle = cpuHandle;
		_DSVDescriptor = dsvDescriptor;
	}


	void SetShaderResourcelView(DescriptorAllocation srvDescriptor, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle)
	{
		_Desc.Usage |= (u32)ResourceUsage::ShaderResource;
		_ShaderResourceViewCPUHandle = cpuHandle;
		_ShaderResourceViewGPUHandle = gpuHandle;
		_SRVDescriptor = srvDescriptor;
	}

private:

	DX12RenderTexture(RenderDevice* device, const Desc& desc, ResourceState state, ComPtr<ID3D12Resource> handle)
	{
		Device = device;
		_Desc = desc;
		State = state;
		_Handle = handle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE _ShaderResourceViewCPUHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE _ShaderResourceViewGPUHandle;
	DescriptorAllocation _SRVDescriptor;

	D3D12_CPU_DESCRIPTOR_HANDLE _RenderTargetViewCPUHandle;
	DescriptorAllocation _RVTDescriptor;

	D3D12_CPU_DESCRIPTOR_HANDLE _DepthStencilViewCPUHandle;
	DescriptorAllocation _DSVDescriptor;
	ComPtr<ID3D12Resource> _Handle;
};

class DX12Sampler : public Sampler
{
	friend class DX12Device;
public:
	~DX12Sampler();

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle()
	{
		return _CpuHandle;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle()
	{
		return _GpuHandle;
	}

private:
	DX12Sampler(const Sampler::Desc& desc, DescriptorAllocation descriptor, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle)
	{
		_Info = desc;
		_CpuHandle = cpuHandle;
		_GpuHandle = gpuHandle;
	}

	DescriptorAllocation _Descriptor;
	D3D12_CPU_DESCRIPTOR_HANDLE _CpuHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE _GpuHandle;
};