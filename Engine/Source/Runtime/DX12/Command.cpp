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

void DX12CommandList::Close(Texture2DResource* presentResource)
{
    DX12Texture2DResource* dx12Res = dynamic_cast<DX12Texture2DResource*>(presentResource);
    if (dx12Res->GetState() != D3D12_RESOURCE_STATE_PRESENT)
    {
        D3D12_RESOURCE_BARRIER barrier = D3D12_RESOURCE_BARRIER{
                .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
                .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
                .Transition = {
                     .pResource = std::any_cast<ID3D12Resource*>(dx12Res->Handle()),
                     .Subresource = 0,
                     .StateBefore = dx12Res->GetState(),
                     .StateAfter = D3D12_RESOURCE_STATE_PRESENT
                }
        };
        dx12Res->SetState(D3D12_RESOURCE_STATE_PRESENT);
        _Handle->ResourceBarrier(1, &barrier);
    }

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

void DX12CommandList::SetRenderTargets(u32 rtNum, Texture2DResource** rts, Texture2DResource* depthStencil)
{
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtViews;
    D3D12_CPU_DESCRIPTOR_HANDLE dsView{};
    D3D12_CPU_DESCRIPTOR_HANDLE* pDepth = nullptr;
    std::vector<D3D12_RESOURCE_BARRIER> rtBarrier;

    if (rtNum > 0 && rts)
    {
        for (u32 i=0; i<rtNum; i++)
        {
            DX12Texture2DResource* dx12Res = dynamic_cast<DX12Texture2DResource*>(rts[i]);
            rtViews.push_back(dx12Res->GetRenderTargetView());
            if (dx12Res->GetState() != D3D12_RESOURCE_STATE_RENDER_TARGET)
            {
                rtBarrier.push_back(D3D12_RESOURCE_BARRIER{
                    .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
                    .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
                    .Transition = {
                         .pResource = std::any_cast<ID3D12Resource*>(dx12Res->Handle()),
                         .Subresource = 0,
                         .StateBefore = dx12Res->GetState(),
                         .StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET
                    }
                    });
                dx12Res->SetState(D3D12_RESOURCE_STATE_RENDER_TARGET);
            }
        }
    }

    if (depthStencil)
    {
        DX12Texture2DResource* dx12depth = dynamic_cast<DX12Texture2DResource*>(depthStencil);
        dsView = dx12depth->GetDepthStencilView();
        pDepth = &dsView;
    }

    if (rtBarrier.size() > 0)
    {
        _Handle->ResourceBarrier((UINT)rtBarrier.size(), rtBarrier.data());
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

void DX12CommandList::ClearRenderTarget(Texture2DResource* renderTarget, const f32* colors)
{
    DX12Texture2DResource* dx12Res = dynamic_cast<DX12Texture2DResource*>(renderTarget);
    _Handle->ClearRenderTargetView(dx12Res->GetRenderTargetView(), colors, 0, nullptr);
}

void DX12CommandList::SetGraphicsRootSignature(RootSignature* signature)
{
    _Handle->SetGraphicsRootSignature(std::any_cast<ID3D12RootSignature*>(signature->Handle()));
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
    ID3D12Resource* srcResource = std::any_cast<ID3D12Resource*>(srcRes->Handle());
    ID3D12Resource* dstResource = std::any_cast<ID3D12Resource*>(dstRes->Handle());
    _Handle->CopyResource(dstResource, srcResource);
    D3D12_RESOURCE_BARRIER barrier = {
        .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
        .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
        .Transition = {
            .pResource = dstResource,
            .Subresource = 0,
            .StateBefore = D3D12_RESOURCE_STATE_COPY_DEST,
            .StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
        }
    };
    _Handle->ResourceBarrier(1, &barrier);
}