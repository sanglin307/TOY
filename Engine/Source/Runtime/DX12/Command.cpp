#include "Private.h"

void DX12Fence::Wait(u32 frameIndex, CommandQueue* commandQueue)
{
    ID3D12CommandQueue* queue = std::any_cast<ID3D12CommandQueue*>(commandQueue->Handle());
    check(SUCCEEDED(queue->Signal(_Handle.Get(), _FenceValues[frameIndex])));

    // Wait until the fence has been processed.
    check(SUCCEEDED(_Handle->SetEventOnCompletion(_FenceValues[frameIndex], _FenceEvent)));
    WaitForSingleObjectEx(_FenceEvent, INFINITE, FALSE);

    _FenceValues[frameIndex]++;
}

void DX12Fence::Advance(u32 frameIndex, u32 nextFrameIndex, CommandQueue* commandQueue)
{
    u64 currentFenceValue = _FenceValues[frameIndex];
    ID3D12CommandQueue* queue = std::any_cast<ID3D12CommandQueue*>(commandQueue->Handle());
    check(SUCCEEDED(queue->Signal(_Handle.Get(), currentFenceValue)));
 
    // If the next frame is not ready to be rendered yet, wait until it is ready.
    if (_Handle->GetCompletedValue() < _FenceValues[nextFrameIndex])
    {
        check(SUCCEEDED(_Handle->SetEventOnCompletion(_FenceValues[nextFrameIndex], _FenceEvent)));
        WaitForSingleObjectEx(_FenceEvent, INFINITE, FALSE);
    }

    _FenceValues[nextFrameIndex] = currentFenceValue + 1;
}


void DX12CommandList::Prepare(CommandAllocator* allocator)
{
    ID3D12CommandAllocator* alloc = std::any_cast<ID3D12CommandAllocator*>(allocator->Handle());
    check(SUCCEEDED(alloc->Reset()));

    check(SUCCEEDED(_Handle->Reset(alloc, nullptr)));
}

void DX12CommandList::End(Texture2DResource* presentResource)
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

void DX12CommandList::SetViewport(const Viewport& viewport)
{
    D3D12_VIEWPORT vp = {
       .TopLeftX = (f32)viewport.X,
       .TopLeftY = (f32)viewport.Y,
       .Width = (f32)viewport.Width,
       .Height = (f32)viewport.Height,
       .MinDepth = viewport.MinDepth,
       .MaxDepth = viewport.MaxDepth
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

void DX12CommandList::SetRenderTargets(std::vector<Texture2DResource*>& rts, Texture2DResource* depthStencil)
{
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtViews;
    D3D12_CPU_DESCRIPTOR_HANDLE dsView{};
    D3D12_CPU_DESCRIPTOR_HANDLE* pDepth = nullptr;
    std::vector<D3D12_RESOURCE_BARRIER> rtBarrier;

    for (auto res : rts)
    {
        DX12Texture2DResource* dx12Res = dynamic_cast<DX12Texture2DResource*>(res);
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

    _Handle->OMSetRenderTargets((UINT)rtViews.size(), rtViews.data(), false, pDepth);

}

void DX12CommandList::ClearRenderTarget(Texture2DResource* renderTarget, std::array<float, 4>& colors)
{
    DX12Texture2DResource* dx12Res = dynamic_cast<DX12Texture2DResource*>(renderTarget);
    _Handle->ClearRenderTargetView(dx12Res->GetRenderTargetView(), colors.data(), 0, nullptr);
}

void DX12CommandList::SetGraphicsRootSignature(RootSignature* signature)
{
    _Handle->SetGraphicsRootSignature(std::any_cast<ID3D12RootSignature*>(signature->Handle()));
}

void DX12CommandQueue::Excute(std::vector<CommandList*> commands)
{
    std::vector<ID3D12CommandList*> commandLists;
    for (CommandList* l : commands)
    {
        commandLists.push_back(std::any_cast<ID3D12GraphicsCommandList*>(l->Handle()));
    }
    _Handle->ExecuteCommandLists((UINT)commandLists.size(), commandLists.data());
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