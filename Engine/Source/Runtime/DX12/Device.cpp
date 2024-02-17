#include "Device.h"
#include "Command.h"
#include "Resource.h"
#include "Utils.h"

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

CommandQueue* DX12Device::CreateCommandQueue(const CommandType type)
{
    assert(_Device);
    D3D12_COMMAND_QUEUE_DESC queueDesc = {
        .Type = DX12Util::TranslateCommandType(type),
        .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE
    };
 
    ID3D12CommandQueue* commandQueue = nullptr;
    if (SUCCEEDED(_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue))))
    {
        return new DX12CommandQueue(type,commandQueue);
    }
     
    assert(0);
    return nullptr;
}

CommandAllocator* DX12Device::CreateCommandAllocator(const CommandType type)
{
    assert(_Device);
    ID3D12CommandAllocator* commandAlloc = nullptr;
    if (SUCCEEDED(_Device->CreateCommandAllocator(DX12Util::TranslateCommandType(type), IID_PPV_ARGS(&commandAlloc))))
    {
        return new DX12CommandAllocator(type,commandAlloc);
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

        //RVT descriptor heap
        ID3D12DescriptorHeap* rvtHeap = nullptr;
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
            .NumDescriptors = config.BufferCount,
            .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE
        };
        assert(SUCCEEDED(_Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rvtHeap))));

        DescriptorHeap::Config hc = {
            .Type = DescriptorType::RVT,
            .Number = config.BufferCount,
            .Size = _Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV),
            .GPUVisible = false,
        };

        DX12DescriptorHeap* descriptorHeap = new DX12DescriptorHeap(hc, rvtHeap);

        std::vector<RenderResource*> rtResources;
        std::vector<RenderTargetView*> rvts;
        rtResources.resize(config.BufferCount);
        rvts.resize(config.BufferCount);
        D3D12_CPU_DESCRIPTOR_HANDLE rvtHandle = rvtHeap->GetCPUDescriptorHandleForHeapStart();
        for (u32 n = 0; n < config.BufferCount; n++)
        {
            ID3D12Resource* res = nullptr;
            assert(SUCCEEDED(swapChain3->GetBuffer(n, IID_PPV_ARGS(&res))));
            _Device->CreateRenderTargetView(res, nullptr, rvtHandle);

       
            rtResources[n] = new DX12Resource(res);
            
            RenderTargetView::Config rc = {
                .Format = config.Format,
                .Dimension = ViewDimension::Texture2D,
            };
            rvts[n] = new DX12RenderTargetView(rtResources[n], rc, descriptorHeap, rvtHandle);
            rvtHandle.ptr += hc.Size;
        }

        return new DX12SwapChain(config,descriptorHeap,rtResources,rvts, swapChain3);
    }


    assert(0);
    return nullptr;
}

RootSignature* DX12Device::CreateRootSignature()
{
    assert(_Device);
    D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {
        .HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1,
    };

    if (FAILED(_Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
    {
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }

    return nullptr;
    //CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
    //CD3DX12_ROOT_PARAMETER1 rootParameters[1];

    //ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
    //rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_VERTEX);

    //// Allow input layout and deny uneccessary access to certain pipeline stages.
    //D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
    //    D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
    //    D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
    //    D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
    //    D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
    //    D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

    //CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
    //rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

    //ComPtr<ID3DBlob> signature;
    //ComPtr<ID3DBlob> error;
    //ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
    //ThrowIfFailed(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));
}