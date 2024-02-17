#include "Resource.h"

u32 DX12SwapChain::CurrentFrameIndex()
{
    assert(_Handle);
    return _Handle->GetCurrentBackBufferIndex();
}

 
 