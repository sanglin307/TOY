#include "Private.h"

std::any DX12DescriptorHeap::Handle()
{
    return _Handle.Get();
}

std::any DX12DescriptorHeap::GetCPUDescriptorHandle(u32 reserve)
{
    check(_Offset + reserve < _Config.Number);
    D3D12_CPU_DESCRIPTOR_HANDLE base = _Handle->GetCPUDescriptorHandleForHeapStart();
    base.ptr += _Offset * _Config.Stride;
    _Offset += reserve;
    return base;
}

std::any DX12DescriptorHeap::GetGPUDescriptorHandle(u32 reserve)
{
    check(_Offset + reserve < _Config.Number);
    D3D12_GPU_DESCRIPTOR_HANDLE base = _Handle->GetGPUDescriptorHandleForHeapStart();
    base.ptr += _Offset * _Config.Stride;
    _Offset += reserve;
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

DescriptorHeap* DX12Device::CreateDescriptorHeap(DescriptorType type, u32 num, bool gpuVisible)
{
    check(_Device);

    D3D12_DESCRIPTOR_HEAP_TYPE heapType = TranslateDescriptorType(type);
    if (heapType == D3D12_DESCRIPTOR_HEAP_TYPE_RTV || heapType == D3D12_DESCRIPTOR_HEAP_TYPE_DSV)
    {
        gpuVisible = false;
    }

    ComPtr<ID3D12DescriptorHeap> heap;
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {
        .Type = heapType,
        .NumDescriptors = num,
        .Flags = gpuVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE
    };
    check(SUCCEEDED(_Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap))));

    DescriptorHeap::Config hc = {
        .Type = type,
        .Number = num,
        .Stride = _Device->GetDescriptorHandleIncrementSize(heapType),
        .GPUVisible = gpuVisible,
    };

    return new DX12DescriptorHeap(hc, heap);
}

RenderTexture* DX12Device::CreateTexture(const RenderTexture::Desc& desc)
{
    return nullptr;
}

RenderBuffer* DX12Device::CreateBuffer(RenderContext* ctx, const RenderBuffer::Desc& info)
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

        if (info.InitData != nullptr)
        {
            u8* pData = nullptr;
            D3D12_RANGE range = {};
            check(SUCCEEDED(resource->Map(0, &range, (void**)&pData)));
            std::memcpy(pData, info.InitData, info.Size);
            resource->Unmap(0, nullptr);
        }
    }
    else
    {
        D3D12_HEAP_PROPERTIES heap = {
                .Type = D3D12_HEAP_TYPE_DEFAULT
        };
        check(SUCCEEDED(_Device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &desc, info.InitData == nullptr ? D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER : D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&resource))));

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
            
            ID3D12GraphicsCommandList* deviceCmdlist = std::any_cast<ID3D12GraphicsCommandList*>(ctx->Handle());
            DX12CommandList* dxCmdlist = dynamic_cast<DX12CommandList*>(ctx);

            deviceCmdlist->CopyResource(resource.Get(), tempRes.Get());
            dxCmdlist->TransitionState(D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_STATE_COPY_DEST, resource.Get());
        }
    }

    resource->SetName(PlatformUtils::UTF8ToUTF16(info.Name).c_str());
    RenderBuffer* buffer = new DX12RenderBuffer(info,resource);
    return buffer;
 
}
 
 