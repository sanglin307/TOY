#pragma once

#include "../Renderer/Pipeline.h"
#include "Defines.h"

class DX12RootSignature : public RootSignature
{
	friend class DX12Device;
public:
	virtual ~DX12RootSignature() { _Handle.Reset(); }

private:
	DX12RootSignature();

	ComPtr<ID3D12RootSignature> _Handle;
};
