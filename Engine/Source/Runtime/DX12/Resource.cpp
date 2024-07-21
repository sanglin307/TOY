#include "Private.h"

 
std::any DX12DescriptorHeap::Handle()
{
    return _Handle.Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE DX12DescriptorHeap::CPUHandle(DescriptorAllocation& pos)
{
    D3D12_CPU_DESCRIPTOR_HANDLE base = _Handle->GetCPUDescriptorHandleForHeapStart();
    base.ptr += pos.Offset * _Stride;
    return base;
}

D3D12_GPU_DESCRIPTOR_HANDLE DX12DescriptorHeap::GPUHandle(DescriptorAllocation& pos)
{
    D3D12_GPU_DESCRIPTOR_HANDLE base = _Handle->GetGPUDescriptorHandleForHeapStart();
    base.ptr += pos.Offset * _Stride;
    return base;
}

std::any DX12DynamicDescriptorHeap::Handle()
{
    return _Handle.Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE DX12DynamicDescriptorHeap::CPUHandle(u32 pos)
{
    D3D12_CPU_DESCRIPTOR_HANDLE base = _Handle->GetCPUDescriptorHandleForHeapStart();
    base.ptr += pos * _Stride;
    return base;
}

D3D12_GPU_DESCRIPTOR_HANDLE DX12DynamicDescriptorHeap::GPUHandle(u32 pos)
{
    D3D12_GPU_DESCRIPTOR_HANDLE base = _Handle->GetGPUDescriptorHandleForHeapStart();
    base.ptr += pos * _Stride;
    return base;
}


void DX12Swapchain::Present(bool vSync)
{
    check(SUCCEEDED(_Handle->Present(vSync ? 1 : 0, 0)));
}

DX12Swapchain::~DX12Swapchain()
{
    for (RenderResource* res : _RenderTargets)
    {
        delete res;
    }
    _RenderTargets.clear();
    _Handle.Reset();
}

D3D12_DESCRIPTOR_HEAP_TYPE TranslateDescriptorType(DescriptorType type)
{
    switch (type)
    {
    case DescriptorType::CBV_SRV_UAV:
        return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    case DescriptorType::Sampler:
        return D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
    case DescriptorType::RTV:
        return D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    case DescriptorType::DSV:
        return D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    default:
        break;
    }

    check(0);
    return D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
}

DescriptorHeap* DX12Device::CreateDescriptorHeap(const DescriptorHeap::Config& c)
{
    check(_Device);
 
    ComPtr<ID3D12DescriptorHeap> heap;
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {
        .Type = TranslateDescriptorType(c.Type),
        .NumDescriptors = c.Number,
        .Flags = c.GPUVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE
    };
    check(SUCCEEDED(_Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap))));

    return new DX12DescriptorHeap(c, _Device->GetDescriptorHandleIncrementSize(heapDesc.Type),heap);
}

DynamicDescriptorHeap* DX12Device::CreateDynamicDescriptorHeap(u32 size, DescriptorType type)
{
    check(_Device);
    check(type == DescriptorType::CBV_SRV_UAV || type == DescriptorType::Sampler);
    ComPtr<ID3D12DescriptorHeap> heap;
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {
        .Type = TranslateDescriptorType(type),
        .NumDescriptors = size,
        .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
    };
    check(SUCCEEDED(_Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap))));

    return new DX12DynamicDescriptorHeap(size, _Device->GetDescriptorHandleIncrementSize(heapDesc.Type), heap);
}

RenderTexture* DX12Device::CreateTexture(const std::string& name, const RenderTexture::Desc& desc)
{
    check(_Device);
    ComPtr<ID3D12Resource> resource;
    D3D12_RESOURCE_DESC texDesc = {
        .Dimension = TranslateResourceDimension(desc.Dimension),
        .Width = desc.Width,
        .Height = desc.Height,
        .DepthOrArraySize = desc.DepthOrArraySize,
        .MipLevels = 1,
        .Format = TranslatePixelFormat(desc.Format),
        .SampleDesc {
            .Count = 1,
            .Quality = 0
        },
        .Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
        .Flags = D3D12_RESOURCE_FLAG_NONE
    };

    D3D12_CLEAR_VALUE clearValueRT = {
   .Format = texDesc.Format,
   .Color = { 0.f,0.f,0.f,1.f}
    };

    D3D12_CLEAR_VALUE clearValueDS = {
        .Format = texDesc.Format,
        .DepthStencil = {
           .Depth = 0,
           .Stencil = 0
        }
    };

    if (desc.Usage & (u32)ResourceUsage::RenderTarget)
    {
        texDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
    }

    if (desc.Usage & (u32)ResourceUsage::DepthStencil)
    {
        texDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        check(IsDepthPixelFormat(desc.Format));
        texDesc.Format = TranslatePixelFormat(GetDepthResourceFormat(desc.Format));
    }

    if (desc.Usage & (u32)ResourceUsage::UnorderedAccess)
    {
        texDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    }

    D3D12_HEAP_PROPERTIES heap = {
                .Type = D3D12_HEAP_TYPE_DEFAULT
    };

    if (desc.Usage & (u32)ResourceUsage::RenderTarget)
    {
        check(SUCCEEDED(_Device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &texDesc, D3D12_RESOURCE_STATE_COMMON, &clearValueRT, IID_PPV_ARGS(&resource))));
    }
    else if (desc.Usage & (u32)ResourceUsage::DepthStencil)
    {
        check(SUCCEEDED(_Device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &texDesc, D3D12_RESOURCE_STATE_COMMON, &clearValueDS, IID_PPV_ARGS(&resource))));
    }
    else
    {
        check(SUCCEEDED(_Device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &texDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&resource))));
    }

    resource->SetName(PlatformUtils::UTF8ToUTF16(name).c_str());

    DX12RenderTexture* dstTexture = nullptr;

    if (desc.Data != nullptr)
    {
        const u64 uploadBufferSize = GetTextureRequiredIntermediateSize(resource.Get(), 0, 1);
        D3D12_RESOURCE_DESC tempDesc = {
            .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
            .Alignment = 0,
            .Width = uploadBufferSize,
            .Height = 1,
            .DepthOrArraySize = 1,
            .MipLevels = 1,
            .Format = DXGI_FORMAT_UNKNOWN,
            .SampleDesc {
                .Count = 1,
                .Quality = 0
            },
            .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR
        };
        ComPtr<ID3D12Resource> tempRes;
        D3D12_HEAP_PROPERTIES uploadHeap = {
               .Type = D3D12_HEAP_TYPE_UPLOAD
        };
        check(SUCCEEDED(_Device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &tempDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&tempRes))));
        D3D12_SUBRESOURCE_DATA textureData = {
            .pData = desc.Data,
            .RowPitch = desc.Width * GetPixelSize(desc.Format)
        };
        textureData.SlicePitch = textureData.RowPitch * desc.Height;

        u64 copyFenceValue = 0;
        DX12CommandList* ctx = static_cast<DX12CommandList*>(_ContextManager->GetCopyContext(copyFenceValue));
        dstTexture = new DX12RenderTexture(name, this, desc, ResourceState::CopyDest, resource);
        RenderBuffer::Desc buffDesc = {
            .Size = uploadBufferSize
        };
        RenderBuffer* srcBuffer = new DX12RenderBuffer("Texture_TempCopy", this, buffDesc, ResourceState::GenericRead, tempRes);
        ctx->UpdateSubresource(dstTexture, srcBuffer, 0, 0, 1, &textureData);
        AddDelayDeleteResource(srcBuffer, DelayDeleteResourceType::CopyQueue, copyFenceValue);
    }
    else
    {
        dstTexture = new DX12RenderTexture(name, this, desc, ResourceState::Common, resource);
    }

    if (desc.Usage & (u32)ResourceUsage::ShaderResource)
    {
        DX12DescriptorHeap* heap = static_cast<DX12DescriptorHeap*>(_DescriptorManager->GetCPUSRVHeap());
        if (IsDepthPixelFormat(desc.Format))
        {
            // depth shader view.
            texDesc.Format = TranslatePixelFormat(GetDepthShaderResourceFormat(desc.Format,true));
            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {
                 .Format = texDesc.Format,
                 .ViewDimension = TranslateResourceViewDimension(desc.Dimension),
                 .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
                 .Texture2D = {
                    .MipLevels = 1,
                    .PlaneSlice = 0
                 }
            };
            DescriptorAllocation srvDescriptor = heap->Allocate();
            D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = heap->CPUHandle(srvDescriptor);
            _Device->CreateShaderResourceView(resource.Get(), &srvDesc, cpuHandle);
            dstTexture->SetSRV(srvDescriptor, cpuHandle);

            // stencil shader view.
            if (IsStencilPixelFormat(desc.Format))
            {
                texDesc.Format = TranslatePixelFormat(GetDepthShaderResourceFormat(desc.Format, false));
                srvDesc = {
                     .Format = texDesc.Format,
                     .ViewDimension = TranslateResourceViewDimension(desc.Dimension),
                     .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
                     .Texture2D = {
                        .MipLevels = 1,
                        .PlaneSlice = 1 // stencil plane is 1.
                     }
                };
                srvDescriptor = heap->Allocate();
                cpuHandle = heap->CPUHandle(srvDescriptor);
                _Device->CreateShaderResourceView(resource.Get(), &srvDesc, cpuHandle);
                dstTexture->SetStencilSRV(srvDescriptor, cpuHandle);
                dstTexture->StencilState = ResourceState::Common;
            }
        }
        else
        {
            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {
                 .Format = texDesc.Format,
                 .ViewDimension = TranslateResourceViewDimension(desc.Dimension),
                 .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
                 .Texture2D = {
                    .MipLevels = 1
                 }
            };
            DescriptorAllocation srvDescriptor = heap->Allocate();
            D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = heap->CPUHandle(srvDescriptor);
            _Device->CreateShaderResourceView(resource.Get(), &srvDesc, cpuHandle);
            dstTexture->SetSRV(srvDescriptor, cpuHandle);
        }
    }

    if (desc.Usage & (u32)ResourceUsage::RenderTarget)
    {
        DX12DescriptorHeap* rvtHeap = static_cast<DX12DescriptorHeap*>(_DescriptorManager->GetGPURTVHeap());
        DescriptorAllocation da = rvtHeap->Allocate();
        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = rvtHeap->CPUHandle(da);
        _Device->CreateRenderTargetView(resource.Get(), nullptr, cpuHandle);
        dstTexture->SetRTV(da, cpuHandle);
    }

    if (desc.Usage & (u32)ResourceUsage::DepthStencil)
    {
        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {
                 .Format = TranslatePixelFormat(desc.Format),
                 .ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D,
                 .Flags = D3D12_DSV_FLAG_NONE,
                 .Texture2D = {
                    .MipSlice = 0,
                  }
        };
        DX12DescriptorHeap* dsvHeap = static_cast<DX12DescriptorHeap*>(_DescriptorManager->GetGPUDSVHeap());
        DescriptorAllocation da = dsvHeap->Allocate();
        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = dsvHeap->CPUHandle(da);
        _Device->CreateDepthStencilView(resource.Get(), &dsvDesc, cpuHandle);
        dstTexture->SetDSV(da, cpuHandle);
    }

    if (desc.Usage & (u32)ResourceUsage::UnorderedAccess)
    {
        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {
             .Format = texDesc.Format,
             .ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D,
             .Texture2D = {
                 .MipSlice = 0,
                 .PlaneSlice = 0
             },
        };

        DX12DescriptorHeap* uavHeap = static_cast<DX12DescriptorHeap*>(_DescriptorManager->GetCPUSRVHeap());
        DescriptorAllocation uav = uavHeap->Allocate();
        D3D12_CPU_DESCRIPTOR_HANDLE uavHandle = uavHeap->CPUHandle(uav);
        _Device->CreateUnorderedAccessView(resource.Get(), nullptr, &uavDesc, uavHandle);
        dstTexture->SetUAV(uav, uavHandle);
    }

    return dstTexture;
}

RenderBuffer* DX12Device::CreateBuffer(const std::string& name,const RenderBuffer::Desc& info)
{
    check(_Device);

    u64 bufferSize = info.Size;
    if (info.Usage & (u32)ResourceUsage::UniformBuffer)
    {
        bufferSize = (bufferSize + 255) & ~255;
    }

    ComPtr<ID3D12Resource> resource;
    D3D12_RESOURCE_DESC desc = {
        .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
        .Alignment = info.Alignment ? D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT : 0ul,
        .Width = bufferSize,
        .Height = 1,
        .DepthOrArraySize = 1,
        .MipLevels = 1,
        .Format = DXGI_FORMAT_UNKNOWN,
        .SampleDesc {
            .Count = 1,
            .Quality = 0
        },
        .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR
    };

    DX12RenderBuffer* dstBuffer = nullptr;
    if ( info.CpuAccess != CpuAccessFlags::None )
    {
        D3D12_HEAP_PROPERTIES heap = {
            .Type = D3D12_HEAP_TYPE_UPLOAD
        };

        check(SUCCEEDED(_Device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource))));
        resource->SetName(PlatformUtils::UTF8ToUTF16(name).c_str());

        if (info.InitData != nullptr)
        {
            u8* pData = nullptr;
            D3D12_RANGE range = {};
            check(SUCCEEDED(resource->Map(0, &range, (void**)&pData)));
            std::memcpy(pData, info.InitData, info.Size);
            resource->Unmap(0, nullptr);
        }

        dstBuffer = new DX12RenderBuffer(name,this, info, ResourceState::GenericRead, resource);
        u8* uniformDataMap = nullptr;
        if (info.Usage & (u32)ResourceUsage::UniformBuffer)
        {
            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {
                 .BufferLocation = resource->GetGPUVirtualAddress(),
                 .SizeInBytes = (UINT)bufferSize
            };
           
            DX12DescriptorHeap* heap = static_cast<DX12DescriptorHeap*>(_DescriptorManager->GetCPUSRVHeap());
            DescriptorAllocation descriptor = heap->Allocate();
            D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = heap->CPUHandle(descriptor);
            _Device->CreateConstantBufferView(&cbvDesc, cpuHandle);

            D3D12_RANGE range = {};
            check(SUCCEEDED(resource->Map(0, &range, (void**)&uniformDataMap)));
            dstBuffer->_UniformDataMapPointer = uniformDataMap;
            dstBuffer->SetCBV(descriptor,cpuHandle);
        }
    }
    else
    {
        D3D12_HEAP_PROPERTIES heap = {
                .Type = D3D12_HEAP_TYPE_DEFAULT
        };
        check(SUCCEEDED(_Device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&resource))));
        resource->SetName(PlatformUtils::UTF8ToUTF16(name).c_str());

        if (info.InitData != nullptr)
        {
            D3D12_HEAP_PROPERTIES uploadHeap = {
                .Type = D3D12_HEAP_TYPE_UPLOAD
            };
            ComPtr<ID3D12Resource> tempRes;
            check(SUCCEEDED(_Device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&tempRes))));

            u8* pData = nullptr;
            D3D12_RANGE range = {};
            check(SUCCEEDED(tempRes->Map(0, &range, (void**)&pData)));
            std::memcpy(pData, info.InitData, info.Size);
            tempRes->Unmap(0, nullptr);
            
            u64 copyFenceValue = 0;
            RenderContext* ctx = _ContextManager->GetCopyContext(copyFenceValue);
            dstBuffer = new DX12RenderBuffer(name,this,info, ResourceState::Common, resource);
            RenderBuffer* srcBuffer = new DX12RenderBuffer("Buffer_CopyTemp",this, info, ResourceState::GenericRead, tempRes);
            ctx->CopyResource(dstBuffer, srcBuffer);
            AddDelayDeleteResource(srcBuffer, DelayDeleteResourceType::CopyQueue, copyFenceValue);
        }
        else
        {
            dstBuffer = new DX12RenderBuffer(name,this,info, ResourceState::Common, resource);
        }
    }

    if (info.Usage & (u32)ResourceUsage::ShaderResource)
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {
                 .Format = info.StructuredBuffer ? DXGI_FORMAT_UNKNOWN : TranslatePixelFormat(info.Format),
                 .ViewDimension = D3D12_SRV_DIMENSION_BUFFER,
                 .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
                 .Buffer = {
                    .FirstElement = 0,
                    .NumElements = UINT(info.Size / info.Stride),
                    .StructureByteStride = info.StructuredBuffer ? info.Stride : 0,
                    .Flags = D3D12_BUFFER_SRV_FLAG_NONE
                 },
        };

        DX12DescriptorHeap* heap = static_cast<DX12DescriptorHeap*>(_DescriptorManager->GetCPUSRVHeap());
        DescriptorAllocation srvDescriptor = heap->Allocate();
        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = heap->CPUHandle(srvDescriptor);
        _Device->CreateShaderResourceView(resource.Get(), &srvDesc, cpuHandle);
        dstBuffer->SetSRV(srvDescriptor, cpuHandle);
    }

    if (info.Usage & (u32)ResourceUsage::UnorderedAccess)
    {
        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {
             .Format = TranslatePixelFormat(info.Format),
             .ViewDimension = D3D12_UAV_DIMENSION_BUFFER,
             .Buffer = {
                  .FirstElement = 0,
                  .NumElements = UINT(info.Size / info.Stride),
                  .StructureByteStride = info.Stride,
                  .CounterOffsetInBytes = 0,
                  .Flags = D3D12_BUFFER_UAV_FLAG_NONE
             },
        };

        DX12DescriptorHeap* heap = static_cast<DX12DescriptorHeap*>(_DescriptorManager->GetCPUSRVHeap());
        DescriptorAllocation uavDescriptor = heap->Allocate();
        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = heap->CPUHandle(uavDescriptor);
        _Device->CreateUnorderedAccessView(resource.Get(), nullptr,&uavDesc, cpuHandle);
        dstBuffer->SetUAV(uavDescriptor, cpuHandle);
    }

    return dstBuffer;
 
}

D3D12_VERTEX_BUFFER_VIEW DX12RenderBuffer::GetVertexBufferView()
{
    check(_Desc.Usage & (u32)ResourceUsage::VertexBuffer);

    return D3D12_VERTEX_BUFFER_VIEW{
        .BufferLocation = _Handle->GetGPUVirtualAddress(),
        .SizeInBytes = (UINT)_Desc.Size,
        .StrideInBytes = _Desc.Stride
    };
}

D3D12_INDEX_BUFFER_VIEW DX12RenderBuffer::GetIndexBufferView()
{
    check(_Desc.Usage & (u32)ResourceUsage::IndexBuffer);
    return D3D12_INDEX_BUFFER_VIEW{
         .BufferLocation = _Handle->GetGPUVirtualAddress(),
         .SizeInBytes = (UINT)_Desc.Size,
         .Format = _Desc.Stride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT
    };
}

void DX12RenderBuffer::UploadData(u8* data, size_t size)
{
    check(data != nullptr && size > 0);
    check((u8)_Desc.CpuAccess & (u8)CpuAccessFlags::Write);
    std::memcpy(_UniformDataMapPointer, data, size);
}
 