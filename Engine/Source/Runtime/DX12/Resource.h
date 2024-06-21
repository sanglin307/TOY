#pragma once

struct DX12FormatInfo
{
	DXGI_FORMAT   DXFormat;
};


class DX12Swapchain : public Swapchain
{
	friend class DX12Device;
public:
	DX12Swapchain(const Swapchain::Desc& info,std::vector<RenderTexture*>& renderTargets, ComPtr<IDXGISwapChain3> handle)
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
	D3D12_CPU_DESCRIPTOR_HANDLE CPUHandle(DescriptorAllocation& pos);
	D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle(DescriptorAllocation& pos);
private:
	DX12DescriptorHeap(const DescriptorHeap::Config& config, u32 stride,ComPtr<ID3D12DescriptorHeap> handle)
	{
		_Config = config;
		_Stride = stride;
		_Handle = handle;
		check(_Config.Number <= cPreAllocateDescriptorMaxNum);

		_FreeBlocks.push_back(Block{
		.Offset = 0,
		.Count = config.Number
			});
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

		if (_Desc.Usage & (u32)ResourceUsage::UniformBuffer && _CBVDescriptor.Offset >= 0)
		{
			_CBVDescriptor.Heap->Free(_CBVDescriptor);
		}

		if (_Desc.Usage & (u32)ResourceUsage::ShaderResource && _SRVDescriptor.Offset >= 0)
		{
			_SRVDescriptor.Heap->Free(_SRVDescriptor);
		}

		_Handle.Reset(); 
	}
	virtual std::any Handle() override { return _Handle.Get(); }
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView();

	D3D12_CPU_DESCRIPTOR_HANDLE GetConstBufferViewCPUHandle()
	{
		return _ConstBufferViewCPUHandle;
	}

	void SetConstBufferView(DescriptorAllocation descriptor, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle)
	{
		_Desc.Usage |= (u32)ResourceUsage::UniformBuffer;
		_CBVDescriptor = descriptor;
		_ConstBufferViewCPUHandle = cpuHandle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceViewCPUHandle()
	{
		check(_Desc.Usage & (u32)ResourceUsage::ShaderResource);
		return _ShaderResourceViewCPUHandle;
	}

	void SetShaderResourcelView(DescriptorAllocation srvDescriptor, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle)
	{
		_Desc.Usage |= (u32)ResourceUsage::ShaderResource;
		_ShaderResourceViewCPUHandle = cpuHandle;
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
	DescriptorAllocation _SRVDescriptor;

	D3D12_CPU_DESCRIPTOR_HANDLE _ConstBufferViewCPUHandle;
	DescriptorAllocation _CBVDescriptor;
	u8* _UniformDataMapPointer;

	ComPtr<ID3D12Resource> _Handle;
};

class DX12RenderTexture : public RenderTexture
{
	friend class DX12Device;
public:
	virtual ~DX12RenderTexture() 
	{
		if (_SRVDescriptor.Offset >= 0)
			_SRVDescriptor.Heap->Free(_SRVDescriptor);

		if (_RVTDescriptor.Offset >= 0)
			_RVTDescriptor.Heap->Free(_RVTDescriptor);

		if (_DSVDescriptor.Offset >= 0)
			_DSVDescriptor.Heap->Free(_DSVDescriptor);

		_Handle.Reset(); 
	}

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


	void SetShaderResourcelView(DescriptorAllocation srvDescriptor, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle)
	{
		_Desc.Usage |= (u32)ResourceUsage::ShaderResource;
		_ShaderResourceViewCPUHandle = cpuHandle;
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
	virtual ~DX12Sampler()
	{
		if (_Descriptor.Offset >= 0)
			_Descriptor.Heap->Free(_Descriptor);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle()
	{
		return _CpuHandle;
	}

private:
	DX12Sampler(const Sampler::Desc& desc, DescriptorAllocation descriptor, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle)
	{
		_Info = desc;
		_CpuHandle = cpuHandle;
	}

	DescriptorAllocation _Descriptor;
	D3D12_CPU_DESCRIPTOR_HANDLE _CpuHandle;
};