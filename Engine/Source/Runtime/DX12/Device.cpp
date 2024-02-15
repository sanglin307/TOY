#include "Device.h"
#include "Command.h"
#include "Resource.h"

#pragma comment(lib, "d3d12")   
#pragma comment(lib, "dxgi")   


void DX12Device::Init()
{
    DX12Util::Init();

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

    if (SUCCEEDED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&_Factory))))
    {
        IDXGIFactory6* factory6 = nullptr;
        if (SUCCEEDED(_Factory->QueryInterface(IID_PPV_ARGS(&factory6))))
        {
            for (u32 index = 0;SUCCEEDED(factory6->EnumAdapterByGpuPreference(index, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&_Adapter))); ++index)
            {
                DXGI_ADAPTER_DESC1 desc;
                _Adapter->GetDesc1(&desc);

                if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                {
                    continue;
                }

                if (SUCCEEDED(D3D12CreateDevice(_Adapter, D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&_Device))))
                {
                    break;
                }
            }
        }
        factory6->Release();

        if (!_Adapter)
        {
            for (u32 index = 0; SUCCEEDED(_Factory->EnumAdapters1(index, &_Adapter)); ++index)
            {
                DXGI_ADAPTER_DESC1 desc;
                _Adapter->GetDesc1(&desc);

                if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                {
                    continue;
                }

                if (SUCCEEDED(D3D12CreateDevice(_Adapter, D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&_Device))))
                {
                    break;
                }
            }
        }
    }
    else
    {
        assert(0);
    }

    assert(_Factory);
    assert(_Adapter);
    assert(_Device);

}

void DX12Device::Destroy()
{
    if (_Device)
    {
        _Device->Release();
        _Device = nullptr;
    }

    if (_Adapter)
    {
        _Adapter->Release();
        _Adapter = nullptr;
    }

    if (_Factory)
    {
        _Factory->Release();
        _Factory = nullptr;
    }
}

CommandQueue* DX12Device::CreateCommandQueue(const CommandQueue::Type type)
{
    assert(_Device);
    D3D12_COMMAND_QUEUE_DESC queueDesc = {
        .Type = DX12CommandQueue::TranslateType(type),
        .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE
    };
 
    ID3D12CommandQueue* commandQueue = nullptr;
    if (SUCCEEDED(_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue))))
    {
        return new DX12CommandQueue(commandQueue);
    }
     
    assert(0);
    return nullptr;
}

SwapChain* DX12Device::CreateSwapChain(const SwapChain::Config& config,CommandQueue* queue,void* hwnd)
{
    assert(_Factory);
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {
        .Width = config.Width,
        .Height = config.Height,
        .Format = DX12Util::TranslateFormat(config.Format),
        .SampleDesc = {.Count = config.SampleCount, .Quality = config.SampleQuality},
        .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .BufferCount = config.BufferCount,
        .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
    };

    DX12CommandQueue* dxQueue = dynamic_cast<DX12CommandQueue*>(queue);
    IDXGISwapChain1* swapChain = nullptr;
    if (SUCCEEDED(_Factory->CreateSwapChainForHwnd(dxQueue->Handle(),
        static_cast<HWND>(hwnd),
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain)
    ))
    {
        _Factory->MakeWindowAssociation(static_cast<HWND>(hwnd), DXGI_MWA_NO_ALT_ENTER);
        IDXGISwapChain3* swapChain3 = nullptr;
        assert(SUCCEEDED(swapChain->QueryInterface(__uuidof(IDXGISwapChain3), ((void**)&swapChain3))));
        swapChain->Release();
        return new DX12SwapChain(config, swapChain3);
    }


    assert(0);
    return nullptr;
}