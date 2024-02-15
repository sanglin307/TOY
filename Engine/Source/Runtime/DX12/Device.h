#pragma once

#include "../Core/Types.h"
#include "../Renderer/Device.h"
#include "Defines.h"


class DX12_API DX12Device : public RenderDevice
{
public :

	virtual void Init() override;
	virtual void Destroy() override;
	virtual CommandQueue* CreateCommandQueue(const CommandQueue::Type type) override;
	virtual SwapChain* CreateSwapChain(const SwapChain::Config& config, CommandQueue* queue, void* hwnd = nullptr) override;

private:
	IDXGIFactory4* _Factory = nullptr;
    IDXGIAdapter1* _Adapter = nullptr;
    ID3D12Device5* _Device = nullptr;
};

