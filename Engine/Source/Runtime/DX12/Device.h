#pragma once

#include "../Core/Types.h"
#include "../Renderer/Device.h"
#include "Defines.h"


class DX12Device : public RenderDevice
{
public :
	DX12_API DX12Device();
	virtual ~DX12Device();
	virtual CommandQueue* CreateCommandQueue(const CommandType type) override;
	virtual CommandAllocator* CreateCommandAllocator(const CommandType type) override;
	virtual SwapChain* CreateSwapChain(const SwapChain::Config& config, CommandQueue* queue, const std::any hwnd = nullptr) override;
	virtual RootSignature* CreateRootSignature() override;
	

private:
	void ReportLiveObjects();

	ComPtr<IDXGIFactory4> _Factory;
	ComPtr<IDXGIAdapter1> _Adapter;
	ComPtr<ID3D12Device5> _Device;
};

