#pragma once

#include "../Renderer/Pipeline.h"
#include "Defines.h"

class DX12RootSignature : public RootSignature
{
	friend class DX12Device;
public:
	virtual ~DX12RootSignature();

private:
	DX12RootSignature();

	ID3D12RootSignature* _Handle = nullptr;
};