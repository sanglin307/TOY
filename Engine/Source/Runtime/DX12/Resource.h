#pragma once

#include "Defines.h"
#include "../Renderer/Resource.h"

struct DX12FormatInfo
{
	DXGI_FORMAT   DXFormat;
};

class DX12SwapChain : public SwapChain
{
	friend class DX12Device;
public:
	virtual ~DX12SwapChain();
	virtual u32 CurrentFrameIndex() override;
private:
	DX12SwapChain(const SwapChain::Config& config, DescriptorHeap* descriptorHeap, std::vector<RenderResource*>& renderTargets,
	              std::vector<RenderTargetView*>& renderTargetViews, ComPtr<IDXGISwapChain3> handle)
	{
		_Config = config;
		_DescriptorHeap = descriptorHeap;
		_RenderTargets = renderTargets;
		_RenderTargetViews = renderTargetViews;
		_Handle = handle;
	}

	ComPtr<IDXGISwapChain3> _Handle;
};

class DX12DescriptorHeap : public DescriptorHeap
{
	friend class DX12Device;
public:
	virtual ~DX12DescriptorHeap() { _Handle.Reset(); }

private:
	DX12DescriptorHeap(const DescriptorHeap::Config& config, ComPtr<ID3D12DescriptorHeap> handle)
	{
		_Config = config;
		_Handle = handle;
	}
	ComPtr<ID3D12DescriptorHeap> _Handle = nullptr;
};

class DX12Resource : public RenderResource
{
	friend class DX12Device;
public:
	virtual ~DX12Resource() { _Handle.Reset(); }
private:

	DX12Resource(ComPtr<ID3D12Resource> handle)
	{
		_Handle = handle;
	}

	ComPtr<ID3D12Resource> _Handle;
};

class DX12RenderTargetView : public RenderTargetView
{
	friend class DX12Device;
public :
	virtual ~DX12RenderTargetView() {};
private:
	DX12RenderTargetView(RenderResource* resource, RenderTargetView::Config& config, DX12DescriptorHeap* descriptorHeap, D3D12_CPU_DESCRIPTOR_HANDLE handle)
	{
		_Resource = resource;
		_Config = config;
		_DescriptorHeap = descriptorHeap;
		_Handle = handle;
	}
	DX12DescriptorHeap* _DescriptorHeap = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE _Handle;
};