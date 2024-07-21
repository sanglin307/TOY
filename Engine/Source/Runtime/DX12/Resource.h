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

class DX12DynamicDescriptorHeap : public DynamicDescriptorHeap
{
	friend class DX12Device;
public:
	virtual ~DX12DynamicDescriptorHeap() { _Handle.Reset(); }
	virtual std::any Handle() override;

	D3D12_CPU_DESCRIPTOR_HANDLE CPUHandle(u32 pos);
	D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle(u32 pos);
private:
	DX12DynamicDescriptorHeap(u32 size, u32 stride, ComPtr<ID3D12DescriptorHeap> handle)
	{
		_Current = 0;
		_Size = size;
		_Stride = stride;
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

		if (_Desc.Usage & (u32)ResourceUsage::UniformBuffer && _CBVDescriptor.Offset >= 0)
		{
			_CBVDescriptor.Heap->Free(_CBVDescriptor);
		}

		if (_Desc.Usage & (u32)ResourceUsage::ShaderResource && _SRVDescriptor.Offset >= 0)
		{
			_SRVDescriptor.Heap->Free(_SRVDescriptor);
		}

		if (_Desc.Usage & (u32)ResourceUsage::UnorderedAccess && _UAVDescriptor.Offset >= 0)
		{
			_UAVDescriptor.Heap->Free(_UAVDescriptor);
		}

		_Handle.Reset(); 
	}
	virtual std::any Handle() override { return _Handle.Get(); }
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView();

	D3D12_CPU_DESCRIPTOR_HANDLE GetCBVCPUHandle()
	{
		return _CBVCPUHandle;
	}

	void SetCBV(DescriptorAllocation descriptor, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle)
	{
		_Desc.Usage |= (u32)ResourceUsage::UniformBuffer;
		_CBVDescriptor = descriptor;
		_CBVCPUHandle = cpuHandle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUHandle()
	{
		check(_Desc.Usage & (u32)ResourceUsage::ShaderResource);
		return _SRVCPUHandle;
	}

	void SetSRV(DescriptorAllocation srvDescriptor, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle)
	{
		_Desc.Usage |= (u32)ResourceUsage::ShaderResource;
		_SRVCPUHandle = cpuHandle;
		_SRVDescriptor = srvDescriptor;
	}

	void SetUAV(DescriptorAllocation uavDescriptor, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle)
	{
		_Desc.Usage |= (u32)ResourceUsage::UnorderedAccess;
		_UAVCPUHandle = cpuHandle;
		_UAVDescriptor = uavDescriptor;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GetUAVCPUHandle()
	{
		check(_Desc.Usage & (u32)ResourceUsage::UnorderedAccess);
		return _UAVCPUHandle;
	}

	virtual void UploadData(u8* data, size_t size) override;

private:
	DX12RenderBuffer(const std::string& name, RenderDevice* device, const RenderBuffer::Desc& desc,ResourceState state, ComPtr<ID3D12Resource> handle)
	{
		_Name = name;
		Device = device;
		State = state;
		_Desc = desc;
		_Handle = handle;
	}

private:
	D3D12_CPU_DESCRIPTOR_HANDLE _SRVCPUHandle;
	DescriptorAllocation _SRVDescriptor;

	D3D12_CPU_DESCRIPTOR_HANDLE _UAVCPUHandle;
	DescriptorAllocation _UAVDescriptor;

	D3D12_CPU_DESCRIPTOR_HANDLE _CBVCPUHandle;
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

		if (_StencilSRVDescriptor.Offset >= 0)
			_StencilSRVDescriptor.Heap->Free(_StencilSRVDescriptor);

		if (_RTVDescriptor.Offset >= 0)
			_RTVDescriptor.Heap->Free(_RTVDescriptor);

		if (_DSVDescriptor.Offset >= 0)
			_DSVDescriptor.Heap->Free(_DSVDescriptor);

		if (_UAVDescriptor.Offset >= 0)
			_UAVDescriptor.Heap->Free(_UAVDescriptor);

		_Handle.Reset(); 
	}

	virtual std::any Handle() override { return _Handle.Get(); } 

	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVCPUHandle() 
	{
		check(_Desc.Usage & (u32)ResourceUsage::RenderTarget);
		return _RTVCPUHandle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GetDSVCPUHandle()
	{
		check(_Desc.Usage & (u32)ResourceUsage::DepthStencil);
		return _DSVCPUHandle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUHandle()
	{
		check(_Desc.Usage & (u32)ResourceUsage::ShaderResource);
		return _SRVCPUHandle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GetUAVCPUHandle()
	{
		check(_Desc.Usage & (u32)ResourceUsage::UnorderedAccess);
		return _UAVCPUHandle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GetStencilSRVCPUHandle()
	{
		check(_Desc.Usage & (u32)ResourceUsage::ShaderResource);
		return _StencilSRVCPUHandle;
	}

	void SetRTV(DescriptorAllocation rvtDescriptor, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle)
	{
		_Desc.Usage |= (u32)ResourceUsage::RenderTarget;
		_RTVCPUHandle = cpuHandle;
		_RTVDescriptor = rvtDescriptor;
	}

	void SetDSV(DescriptorAllocation dsvDescriptor, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle)
	{
		_Desc.Usage |= (u32)ResourceUsage::DepthStencil;
		_DSVCPUHandle = cpuHandle;
		_DSVDescriptor = dsvDescriptor;
	}


	void SetSRV(DescriptorAllocation srvDescriptor, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle)
	{
		_Desc.Usage |= (u32)ResourceUsage::ShaderResource;
		_SRVCPUHandle = cpuHandle;
		_SRVDescriptor = srvDescriptor;
	}

	void SetStencilSRV(DescriptorAllocation srvDescriptor, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle)
	{
		_Desc.Usage |= (u32)ResourceUsage::ShaderResource;
		_StencilSRVCPUHandle = cpuHandle;
		_StencilSRVDescriptor = srvDescriptor;
	}

	void SetUAV(DescriptorAllocation uavDescriptor, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle)
	{
		_Desc.Usage |= (u32)ResourceUsage::UnorderedAccess;
		_UAVCPUHandle = cpuHandle;
		_UAVDescriptor = uavDescriptor;
	}

private:

	DX12RenderTexture(const std::string& name, RenderDevice* device, const Desc& desc, ResourceState state, ComPtr<ID3D12Resource> handle)
	{
		_Name = name;
		Device = device;
		_Desc = desc;
		State = state;
		_Handle = handle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE _SRVCPUHandle;
	DescriptorAllocation _SRVDescriptor;

	D3D12_CPU_DESCRIPTOR_HANDLE _UAVCPUHandle;
	DescriptorAllocation _UAVDescriptor;

	// only valid for stencil buffer.
	D3D12_CPU_DESCRIPTOR_HANDLE _StencilSRVCPUHandle;
	DescriptorAllocation _StencilSRVDescriptor;

	D3D12_CPU_DESCRIPTOR_HANDLE _RTVCPUHandle;
	DescriptorAllocation _RTVDescriptor;

	D3D12_CPU_DESCRIPTOR_HANDLE _DSVCPUHandle;
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