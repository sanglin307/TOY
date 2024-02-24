#include "Resource.h"

u32 DX12SwapChain::CurrentFrameIndex()
{
    assert(_Handle);
    return _Handle->GetCurrentBackBufferIndex();
}

DX12SwapChain::~DX12SwapChain()
{
    if (_DescriptorHeap)
    {
        delete _DescriptorHeap;
        _DescriptorHeap = nullptr;
    }

    for (RenderResource* res : _RenderTargets)
    {
        delete res;
    }
    _RenderTargets.clear();

    for (RenderTargetView* view : _RenderTargetViews)
    {
        delete view;
    }
    _RenderTargetViews.clear();
    _Handle.Reset();
}
 
 