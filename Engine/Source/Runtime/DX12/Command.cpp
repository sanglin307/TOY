#include "Private.h"

void DX12Fence::CpuWait(u64 fenceValue)
{
    // Wait until the fence has been processed.
    check(SUCCEEDED(_Handle->SetEventOnCompletion(fenceValue, _FenceEvent)));
    WaitForSingleObjectEx(_FenceEvent, INFINITE, FALSE);
}

void DX12CommandList::Reset()
{
    check(_State == ContextState::Close);
    ID3D12CommandAllocator* alloc = std::any_cast<ID3D12CommandAllocator*>(_Allocator->Handle());
    check(SUCCEEDED(alloc->Reset()));
    check(SUCCEEDED(_Handle->Reset(alloc, nullptr)));
    _State = ContextState::Open;
}

void DX12CommandList::Close(RenderTexture* presentResource)
{
    check(_State == ContextState::Open);
    TransitionState(ResourceState::Present, presentResource);
    check(SUCCEEDED(_Handle->Close()));
    _State = ContextState::Close;
}

void DX12CommandList::Close()
{
    check(_State == ContextState::Open);
    check(SUCCEEDED(_Handle->Close()));
    _State = ContextState::Close;
}

void DX12CommandList::SetGraphicPipeline(GraphicPipeline* pipeline)
{
    // root signature
    RootSignature* rootsig = pipeline->GetRootSignature();
    ID3D12RootSignature* rs = std::any_cast<ID3D12RootSignature*>(rootsig->Handle());
    check(rs);
    _Handle->SetGraphicsRootSignature(rs);

    //descriptor heap
    DescriptorHeap* desHeap = _Device->_DescriptorManager->GetHeap(DescriptorType::CBV_SRV_UAV);
    DescriptorHeap* samplerHeap = _Device->_DescriptorManager->GetHeap(DescriptorType::Sampler);
    ID3D12DescriptorHeap* ppHeap[] = { std::any_cast<ID3D12DescriptorHeap*>(desHeap->Handle()), std::any_cast<ID3D12DescriptorHeap*>(samplerHeap->Handle()) };
    _Handle->SetDescriptorHeaps(2, ppHeap);

    pipeline->BindParameter(this);

    // topology
    _Handle->IASetPrimitiveTopology(DX12Device::TranslatePrimitiveTopology(pipeline->Info.Topology));
    // pso
    ID3D12PipelineState* ps = std::any_cast<ID3D12PipelineState*>(pipeline->Handle());
    _Handle->SetPipelineState(ps);
 
}

void DX12CommandList::SetGraphicShaderParameter(const ShaderParameter* param)
{
    if (param->BindType == ShaderBindType::RootCBV)
    {
        ID3D12Resource* res = std::any_cast<ID3D12Resource*>(param->Resource->Handle());
        _Handle->SetGraphicsRootConstantBufferView(param->RootParamIndex, res->GetGPUVirtualAddress());
    }
    else if (param->BindType == ShaderBindType::RootSRV)
    {
        ID3D12Resource* res = std::any_cast<ID3D12Resource*>(param->Resource->Handle());
        _Handle->SetGraphicsRootShaderResourceView(param->RootParamIndex, res->GetGPUVirtualAddress());
    }
    else if (param->BindType == ShaderBindType::RootUAV)
    {
        ID3D12Resource* res = std::any_cast<ID3D12Resource*>(param->Resource->Handle());
        _Handle->SetGraphicsRootUnorderedAccessView(param->RootParamIndex, res->GetGPUVirtualAddress());
    }
    else
    {
        if (param->BindType == ShaderBindType::TableSampler)
        {
            DX12Sampler* sampler = static_cast<DX12Sampler*>(param->Resource);
            _Handle->SetGraphicsRootDescriptorTable(param->RootParamIndex, sampler->GetGPUHandle());
        }
        else
        {
            if (param->Resource->GetDimension() == ResourceDimension::Buffer)
            {
                DX12RenderBuffer* buffer = static_cast<DX12RenderBuffer*>(param->Resource);
                D3D12_GPU_DESCRIPTOR_HANDLE handle;
                if (buffer->GetUsage() & (u32)ResourceUsage::UniformBuffer)
                {
                    handle = buffer->GetConstBufferViewGPUHandle();
                }
                else if (buffer->GetUsage() & (u32)ResourceUsage::ShaderResource)
                {
                    handle = buffer->GetShaderResourceViewGPUHandle();
                }
                else
                {
                    check(0);
                }
                _Handle->SetGraphicsRootDescriptorTable(param->RootParamIndex, handle);
            }
            else
            {
                DX12RenderTexture* tex = static_cast<DX12RenderTexture*>(param->Resource);
                if (tex->GetUsage() & (u32)ResourceUsage::ShaderResource)
                {
                    D3D12_GPU_DESCRIPTOR_HANDLE handle = tex->GetShaderResourceViewGPUHandle();
                    _Handle->SetGraphicsRootDescriptorTable(param->RootParamIndex, handle);
                }
                else
                {
                    check(0);
                }
            }
        }
    }

}

void DX12CommandList::DrawInstanced(u32 vbNum, RenderBuffer** vbs, u32 instanceCount, u32 vertexOffset, u32 instanceOffset)
{
    check(vbNum > 0);
    std::vector<D3D12_VERTEX_BUFFER_VIEW> vbv;
    for (u32 i = 0; i < vbNum; i++)
    {
        DX12RenderBuffer* buffer = static_cast<DX12RenderBuffer*>(vbs[i]);
        vbv.push_back(buffer->GetVertexBufferView());
    }

    u32 vertexPerInstance = (u32)(vbs[0]->GetSize() / vbs[0]->GetStride());
    _Handle->IASetVertexBuffers(0, (UINT)vbv.size(), vbv.data());
    _Handle->DrawInstanced(vertexPerInstance, instanceCount, vertexOffset, instanceOffset);
}

void DX12CommandList::SetViewport(u32 x, u32 y, u32 width, u32 height, f32 minDepth, f32 maxDepth)
{
    D3D12_VIEWPORT vp = {
       .TopLeftX = (f32)x,
       .TopLeftY = (f32)y,
       .Width = (f32)width,
       .Height = (f32)height,
       .MinDepth = minDepth,
       .MaxDepth = maxDepth
    };

    _Handle->RSSetViewports(1, &vp);
}

void DX12CommandList::SetScissorRect(u32 left, u32 top, u32 right, u32 bottom)
{
    D3D12_RECT rect = {
        .left = (LONG)left,
        .top = (LONG)top,
        .right = (LONG)right,
        .bottom = (LONG)bottom
    };

    _Handle->RSSetScissorRects(1, &rect);
}

void DX12CommandList::SetRenderTargets(u32 rtNum, RenderTexture** rts, RenderTexture* depthStencil)
{
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtViews;
    D3D12_CPU_DESCRIPTOR_HANDLE dsView{};
    D3D12_CPU_DESCRIPTOR_HANDLE* pDepth = nullptr;
    
    if (rtNum > 0 && rts)
    {
        for (u32 i=0; i<rtNum; i++)
        {
            DX12RenderTexture* dx12Res = static_cast<DX12RenderTexture*>(rts[i]);
            rtViews.push_back(dx12Res->GetRenderTargetViewCPUHandle());
        }
        TransitionState(ResourceState::RenderTarget, (RenderResource**)rts, rtNum);
    }

    if (depthStencil)
    {
        DX12RenderTexture* dx12depth = static_cast<DX12RenderTexture*>(depthStencil);
        dsView = dx12depth->GetDepthStencilViewCPUHandle();
        pDepth = &dsView;
    }

    if (rtViews.size() > 0)
    {
        _Handle->OMSetRenderTargets((UINT)rtViews.size(), rtViews.data(), false, pDepth);
    }
    else
    {
        _Handle->OMSetRenderTargets(0, nullptr, false, pDepth);
    }

}

void DX12CommandList::ClearRenderTarget(RenderTexture* renderTarget, const f32* colors)
{
    DX12RenderTexture* dx12Res = static_cast<DX12RenderTexture*>(renderTarget);
    _Handle->ClearRenderTargetView(dx12Res->GetRenderTargetViewCPUHandle(), colors, 0, nullptr);
}


void DX12CommandQueue::Excute(u32 ctxNum, RenderContext** ctx)
{
    std::vector<ID3D12CommandList*> commandLists;
    for (u32 i=0; i< ctxNum; i++)
    {
        commandLists.push_back(std::any_cast<ID3D12GraphicsCommandList*>(ctx[i]->Handle()));
    }
    _Handle->ExecuteCommandLists((UINT)commandLists.size(), commandLists.data());
}

void DX12CommandQueue::Signal(Fence* fence, u64 value)
{
    ID3D12Fence* fenceObj = std::any_cast<ID3D12Fence*>(fence->Handle());
    check(SUCCEEDED(_Handle->Signal(fenceObj, value)));
}

void DX12CommandQueue::Wait(Fence* fence, u64 value)
{
    ID3D12Fence* fenceObj = std::any_cast<ID3D12Fence*>(fence->Handle());
    check(SUCCEEDED(_Handle->Wait(fenceObj, value)));
}

void DX12CommandList::UpdateSubresource(RenderTexture* destResource, RenderBuffer* tempResource, u64 tempResOffset, u32 firstSubresource, u32 numSubresources, const D3D12_SUBRESOURCE_DATA* srcData)
{
    if (!((u32)tempResource->State & (u32)ResourceState::CopySource))
    {
        TransitionState(ResourceState::CopySource, tempResource);
    }

    if (!((u32)destResource->State & (u32)ResourceState::CopyDest))
    {
        TransitionState(ResourceState::CopyDest, destResource);
    }

    ID3D12Resource* dstResource = std::any_cast<ID3D12Resource*>(destResource->Handle());
    ID3D12Resource* srcResource = std::any_cast<ID3D12Resource*>(tempResource->Handle());

    u64 requiredSize = 0;
    u64 tempAllocSize = static_cast<u64>(sizeof(D3D12_PLACED_SUBRESOURCE_FOOTPRINT) + sizeof(u32) + sizeof(u64)) * numSubresources;
    void* tempMem = std::malloc(tempAllocSize);

    D3D12_PLACED_SUBRESOURCE_FOOTPRINT* pLayouts = static_cast<D3D12_PLACED_SUBRESOURCE_FOOTPRINT*>(tempMem);
    u64* pRowSizesInBytes = reinterpret_cast<u64*>(pLayouts + numSubresources);
    auto pNumRows = reinterpret_cast<u32*>(pRowSizesInBytes + numSubresources);

    auto Desc = dstResource->GetDesc();
    _Device->_Device->GetCopyableFootprints(&Desc, firstSubresource, numSubresources, tempResOffset, pLayouts, pNumRows, pRowSizesInBytes, &requiredSize);

    auto tempDesc = srcResource->GetDesc();
    u8* pData;
    HRESULT hr = srcResource->Map(0, nullptr, reinterpret_cast<void**>(&pData));
    for (u32 i = 0; i < numSubresources; ++i)
    {
        D3D12_MEMCPY_DEST DestData = { pData + pLayouts[i].Offset, pLayouts[i].Footprint.RowPitch, SIZE_T(pLayouts[i].Footprint.RowPitch) * SIZE_T(pNumRows[i]) };
        for (u32 z = 0; z < pLayouts[i].Footprint.Depth; ++z)
        {
            auto pDestSlice = static_cast<u8*>(DestData.pData) + DestData.SlicePitch * z;
            auto pSrcSlice = static_cast<const u8*>(srcData[i].pData) + srcData[i].SlicePitch * u64(z);
            for (u32 y = 0; y < pNumRows[i]; ++y)
            {
                std::memcpy(pDestSlice + DestData.RowPitch * y,
                    pSrcSlice + srcData[i].RowPitch * u64(y),
                    pRowSizesInBytes[i]);
            }
        }
    }
    srcResource->Unmap(0, nullptr);

    if (Desc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
    {
        _Handle->CopyBufferRegion(dstResource, 0, srcResource, pLayouts[0].Offset, pLayouts[0].Footprint.Width);
    }
    else
    {
        for (u32 i = 0; i < numSubresources; ++i)
        {
            D3D12_TEXTURE_COPY_LOCATION dst = {
                 .pResource = dstResource,
                 .Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
                 .SubresourceIndex = i + firstSubresource
            };
            D3D12_TEXTURE_COPY_LOCATION src = {
                .pResource = srcResource,
                .Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,
                .PlacedFootprint = pLayouts[i]
            };

            _Handle->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
        }
    }

    std::free(tempMem);
    _Manager->AddCopyNum();
}


void DX12CommandList::CopyResource(RenderResource* dstRes, RenderResource* srcRes)
{
    check(srcRes->GetDimension() == dstRes->GetDimension());
     
    if (!((u32)srcRes->State & (u32)ResourceState::CopySource))
    {
        TransitionState(ResourceState::CopySource, srcRes);
    }

    if (!((u32)dstRes->State & (u32)ResourceState::CopyDest))
    {
        TransitionState(ResourceState::CopyDest,dstRes);
    }

    ID3D12Resource* srcResource = std::any_cast<ID3D12Resource*>(srcRes->Handle());
    ID3D12Resource* dstResource = std::any_cast<ID3D12Resource*>(dstRes->Handle());
    _Handle->CopyResource(dstResource, srcResource);
    _Manager->AddCopyNum();

}

void DX12CommandList::TransitionState(D3D12_RESOURCE_STATES destState, D3D12_RESOURCE_STATES srcState, ID3D12Resource* resource)
{
    D3D12_RESOURCE_BARRIER barrier = {
                .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
                .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
                .Transition = {
                    .pResource = resource,
                    .Subresource = 0,
                    .StateBefore = srcState,
                    .StateAfter = destState
                }
    };
    _Handle->ResourceBarrier(1, &barrier);
}
 
void DX12CommandList::TransitionState(ResourceState destState, RenderResource** resources, u32 number)
{
    std::vector<D3D12_RESOURCE_BARRIER> rtBarrier;
    for (u32 i = 0; i < number; i++)
    {
        if (resources[i]->State != destState)
        {
            rtBarrier.push_back(D3D12_RESOURCE_BARRIER{
                    .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
                    .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
                    .Transition = {
                         .pResource = std::any_cast<ID3D12Resource*>(resources[i]->Handle()),
                         .Subresource = 0,
                         .StateBefore = DX12Device::TranslateResourceState(resources[i]->State),
                         .StateAfter = DX12Device::TranslateResourceState(destState)
                    }
                });

            resources[i]->State = destState;
        }
    }
   
    if (rtBarrier.size() > 0)
    {
        _Handle->ResourceBarrier((UINT)rtBarrier.size(), rtBarrier.data());
    }
}

void DX12CommandList::TransitionState(ResourceState destState, RenderResource* res)
{
    if (res->State != destState)
    {
        TransitionState(DX12Device::TranslateResourceState(destState), DX12Device::TranslateResourceState(res->State), std::any_cast<ID3D12Resource*>(res->Handle()));
        res->State = destState;
    }
}