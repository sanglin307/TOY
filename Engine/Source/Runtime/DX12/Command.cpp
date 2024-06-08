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
            DX12RenderTexture* dx12Res = dynamic_cast<DX12RenderTexture*>(rts[i]);
            rtViews.push_back(dx12Res->GetRenderTargetView());
        }
        TransitionState(ResourceState::RenderTarget, (RenderResource**)rts, rtNum);
    }

    if (depthStencil)
    {
        DX12RenderTexture* dx12depth = dynamic_cast<DX12RenderTexture*>(depthStencil);
        dsView = dx12depth->GetDepthStencilView();
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
    DX12RenderTexture* dx12Res = dynamic_cast<DX12RenderTexture*>(renderTarget);
    _Handle->ClearRenderTargetView(dx12Res->GetRenderTargetView(), colors, 0, nullptr);
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