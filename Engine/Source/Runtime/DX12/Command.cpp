#include "Private.h"

void DX12Fence::CpuWait(u64 fenceValue)
{
    // Wait until the fence has been processed.
    check(SUCCEEDED(_Handle->SetEventOnCompletion(fenceValue, _FenceEvent)));
    WaitForSingleObjectEx(_FenceEvent, INFINITE, FALSE);
}

void DX12CommandList::Reset()
{
    ID3D12CommandAllocator* alloc = std::any_cast<ID3D12CommandAllocator*>(_Allocator->Handle());
    check(SUCCEEDED(alloc->Reset()));
    check(SUCCEEDED(_Handle->Reset(alloc, nullptr)));
}

void DX12CommandList::Close(RenderTexture* presentResource)
{
    DX12RenderTexture* dx12Res = dynamic_cast<DX12RenderTexture*>(presentResource);
    TransitionState(D3D12_RESOURCE_STATE_PRESENT, dx12Res);

    check(SUCCEEDED(_Handle->Close()));
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
    std::vector<DX12RenderTexture*> textures;

    if (rtNum > 0 && rts)
    {
        for (u32 i=0; i<rtNum; i++)
        {
            DX12RenderTexture* dx12Res = dynamic_cast<DX12RenderTexture*>(rts[i]);
            rtViews.push_back(dx12Res->GetRenderTargetView());
            textures.push_back(dx12Res);
        }
        TransitionState(D3D12_RESOURCE_STATE_RENDER_TARGET, textures.data(), (u32)textures.size());
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

void DX12CommandList::CopyResource(RenderResource* dstRes, RenderResource* srcRes)
{
    check(srcRes->GetDimension() == dstRes->GetDimension());
    if (dstRes->GetDimension() == ResourceDimension::Buffer)
    {
        DX12RenderBuffer* srcBuffer = dynamic_cast<DX12RenderBuffer*>(srcRes);
        DX12RenderBuffer* dstBuffer = dynamic_cast<DX12RenderBuffer*>(dstRes);
        TransitionState(D3D12_RESOURCE_STATE_COPY_SOURCE, srcBuffer);
        TransitionState(D3D12_RESOURCE_STATE_COPY_DEST, dstBuffer);
    }
    else
    {
        DX12RenderTexture* srcTex = dynamic_cast<DX12RenderTexture*>(srcRes);
        DX12RenderTexture* dstTex = dynamic_cast<DX12RenderTexture*>(dstRes);
        TransitionState(D3D12_RESOURCE_STATE_COPY_SOURCE, srcTex);
        TransitionState(D3D12_RESOURCE_STATE_COPY_DEST, dstTex);
    }

    ID3D12Resource* srcResource = std::any_cast<ID3D12Resource*>(srcRes->Handle());
    ID3D12Resource* dstResource = std::any_cast<ID3D12Resource*>(dstRes->Handle());
    _Handle->CopyResource(dstResource, srcResource);

   
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

void DX12CommandList::TransitionState(D3D12_RESOURCE_STATES* destStates, D3D12_RESOURCE_STATES* srcState, const ID3D12Resource** resource, u32 number)
{
    check(0);
}

void DX12CommandList::TransitionState(D3D12_RESOURCE_STATES destState, DX12RenderTexture* texture)
{
    if (texture->GetState() != destState)
    {
        TransitionState(destState, texture->GetState(), std::any_cast<ID3D12Resource*>(texture->Handle()));
        texture->SetState(destState);
    }
}

void DX12CommandList::TransitionState(D3D12_RESOURCE_STATES destState, DX12RenderTexture** texture, u32 number)
{
    std::vector<D3D12_RESOURCE_BARRIER> rtBarrier;
    for (u32 i = 0; i < number; i++)
    {
        if (texture[i]->GetState() != destState)
        {
            rtBarrier.push_back(D3D12_RESOURCE_BARRIER{
                    .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
                    .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
                    .Transition = {
                         .pResource = std::any_cast<ID3D12Resource*>(texture[i]->Handle()),
                         .Subresource = 0,
                         .StateBefore = texture[i]->GetState(),
                         .StateAfter = destState
                    }
                });
            texture[i]->SetState(destState);
        }
    }
   
    if (rtBarrier.size() > 0)
    {
        _Handle->ResourceBarrier((UINT)rtBarrier.size(), rtBarrier.data());
    }
}

void DX12CommandList::TransitionState(D3D12_RESOURCE_STATES destState, DX12RenderBuffer* buffer)
{
    if (buffer->GetState() != destState)
    {
        TransitionState(destState, buffer->GetState(), std::any_cast<ID3D12Resource*>(buffer->Handle()));
        buffer->SetState(destState);
    }
}