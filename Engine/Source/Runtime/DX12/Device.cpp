#include "Device.h"
#include <assert.h>

#pragma comment(lib, "d3d12")   
#pragma comment(lib, "dxgi")   


void DX12Device::Init()
{
    u32 dxgiFactoryFlags = 0;

#if defined(_DEBUG)
    {
        ID3D12Debug* debug = nullptr;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug))))
        {
            debug->EnableDebugLayer();
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
            debug->Release();
        }
    }
#endif

    IDXGIFactory4* factory = nullptr;
    if (SUCCEEDED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory))))
    {
        IDXGIFactory6* factory6 = nullptr;
        if (SUCCEEDED(factory->QueryInterface(IID_PPV_ARGS(&factory6))))
        {
            for (u32 index = 0;SUCCEEDED(factory6->EnumAdapterByGpuPreference( index, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&Adapter))); ++index)
            {
                DXGI_ADAPTER_DESC1 desc;
                Adapter->GetDesc1(&desc);

                if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                {
                    continue;
                }

                if (SUCCEEDED(D3D12CreateDevice(Adapter, D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&Device))))
                {
                    break;
                }
            }
        }
        factory6->Release();

        if (Adapter == nullptr)
        {
            for (u32 Index = 0; SUCCEEDED(factory->EnumAdapters1(Index, &Adapter)); ++Index)
            {
                DXGI_ADAPTER_DESC1 desc;
                Adapter->GetDesc1(&desc);

                if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                {
                    continue;
                }

                if (SUCCEEDED(D3D12CreateDevice(Adapter, D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&Device))))
                {
                    break;
                }
            }
        }

        factory->Release();
    }
    else
    {
        assert(0);
    }

    assert(Adapter);
    assert(Device);
}

void DX12Device::Destroy()
{

    if (Device)
    {
        Device->Release();
        Device = nullptr;
    }

    if (Adapter)
    {
        Adapter->Release();
        Adapter = nullptr;
    }


}