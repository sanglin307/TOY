#pragma once

#include "../Core/Types.h"
#include "../Renderer/Device.h"
#include "Defines.h"

#include <d3d12.h>
#include <dxgi1_6.h>

class DX12_API DX12Device
{
public :

    static DX12Device& Instance()
    {
        static DX12Device Inst;
        return Inst;
    }

	void Init();
	void Destroy();


private:
    DX12Device() = default;
    DX12Device(const DX12Device& rhs) = delete;
    DX12Device(DX12Device&& rhs) = delete;
    DX12Device& operator=(const DX12Device& rhs) = delete;
    DX12Device& operator=(DX12Device&& rhs) = delete;

private:
    IDXGIAdapter1* Adapter = nullptr;
    ID3D12Device5* Device = nullptr;

};

