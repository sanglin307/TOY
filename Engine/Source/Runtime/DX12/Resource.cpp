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
    case DescriptorType::RVT:
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

RenderTexture* DX12Device::CreateTexture(const RenderTexture::Desc& desc)
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

    if (desc.Usage & (u32)ResourceUsage::RenderTarget)
    {
        texDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
    }

    if (desc.Usage & (u32)ResourceUsage::DepthStencil)
    {
        texDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    }

    if (desc.Usage & (u32)ResourceUsage::UnorderedAccess)
    {
        texDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    }

    D3D12_HEAP_PROPERTIES heap = {
                .Type = D3D12_HEAP_TYPE_DEFAULT
    };
    check(SUCCEEDED(_Device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &texDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&resource))));
    resource->SetName(PlatformUtils::UTF8ToUTF16(desc.Name).c_str());

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

    DX12CommandList* ctx = static_cast<DX12CommandList*>(_ContextManager->GetCopyContext());
    DX12RenderTexture* dstTexture = new DX12RenderTexture(this, desc, ResourceState::CopyDest, resource);
    RenderBuffer::Desc buffDesc = {
        .Size = uploadBufferSize
    };
    RenderBuffer* srcBuffer = new DX12RenderBuffer(this, buffDesc, ResourceState::GenericRead, tempRes);
    ctx->UpdateSubresource(dstTexture, srcBuffer, 0, 0, 1, &textureData);
    AddDelayDeleteResource(srcBuffer);

    if (desc.Usage & (u32)ResourceUsage::ShaderResource)
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {
             .Format = texDesc.Format,
             .ViewDimension = TranslateResourceViewDimension(desc.Dimension),
             .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
             .Texture2D = {
                .MipLevels = desc.MipLevels
             }
        };

        DX12DescriptorHeap* heap = static_cast<DX12DescriptorHeap*>(_DescriptorManager->GetCPUHeap(DescriptorType::CBV_SRV_UAV));
        DescriptorAllocation srvDescriptor = heap->Allocate();
        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = heap->CPUHandle(srvDescriptor);
        _Device->CreateShaderResourceView(resource.Get(), &srvDesc, cpuHandle);
        dstTexture->SetShaderResourcelView(srvDescriptor, cpuHandle);
    }

    return dstTexture;
}

RenderBuffer* DX12Device::CreateBuffer(const RenderBuffer::Desc& info)
{
    check(_Device);

    ComPtr<ID3D12Resource> resource;
    D3D12_RESOURCE_DESC desc = {
        .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
        .Alignment = info.Alignment ? D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT : 0ul,
        .Width = info.Size,
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

    if ( info.CpuAccess & (u32)CpuAccessFlags::ReadWrite )
    {
        D3D12_HEAP_PROPERTIES heap = {
            .Type = D3D12_HEAP_TYPE_UPLOAD
        };

        check(SUCCEEDED(_Device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource))));
        resource->SetName(PlatformUtils::UTF8ToUTF16(info.Name).c_str());

        if (info.InitData != nullptr)
        {
            u8* pData = nullptr;
            D3D12_RANGE range = {};
            check(SUCCEEDED(resource->Map(0, &range, (void**)&pData)));
            std::memcpy(pData, info.InitData, info.Size);
            resource->Unmap(0, nullptr);
        }

        DX12RenderBuffer* renderBuffer = new DX12RenderBuffer(this, info, ResourceState::GenericRead, resource);
        u8* uniformDataMap = nullptr;
        if (info.Usage & (u32)ResourceUsage::UniformBuffer)
        {
            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {
                 .BufferLocation = resource->GetGPUVirtualAddress(),
                 .SizeInBytes = (UINT)info.Size
            };
           
            DX12DescriptorHeap* heap = static_cast<DX12DescriptorHeap*>(_DescriptorManager->GetCPUHeap(DescriptorType::CBV_SRV_UAV));
            DescriptorAllocation descriptor = heap->Allocate();
            D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = heap->CPUHandle(descriptor);
            _Device->CreateConstantBufferView(&cbvDesc, cpuHandle);

            D3D12_RANGE range = {};
            check(SUCCEEDED(resource->Map(0, &range, (void**)&uniformDataMap)));
            renderBuffer->_UniformDataMapPointer = uniformDataMap;
            renderBuffer->SetConstBufferView(descriptor,cpuHandle);
        }

        return renderBuffer;
    }
    else
    {
        D3D12_HEAP_PROPERTIES heap = {
                .Type = D3D12_HEAP_TYPE_DEFAULT
        };
        check(SUCCEEDED(_Device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&resource))));
        resource->SetName(PlatformUtils::UTF8ToUTF16(info.Name).c_str());

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
            
            RenderContext* ctx = _ContextManager->GetCopyContext();
            RenderBuffer* dstBuffer = new DX12RenderBuffer(this,info, ResourceState::Common, resource);
            RenderBuffer* srcBuffer = new DX12RenderBuffer(this,info, ResourceState::GenericRead, tempRes);
            ctx->CopyResource(dstBuffer, srcBuffer);
            AddDelayDeleteResource(srcBuffer);
            return dstBuffer;
        }
        else
        {
            return new DX12RenderBuffer(this,info, ResourceState::Common, resource);
        }
    }

    return nullptr;
 
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

void DX12RenderBuffer::UploadData(u8* data, size_t size)
{
    check(data != nullptr && size > 0);
    check(_Desc.CpuAccess & (u32)CpuAccessFlags::Write);
    std::memcpy(_UniformDataMapPointer, data, size);
}
 