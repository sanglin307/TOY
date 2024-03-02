#include "Private.h"

#pragma comment(lib, "d3d12")   
#pragma comment(lib, "dxgi")  

#ifdef _DEBUG
#pragma comment(lib, "dxguid")   
#endif

DX12Device::DX12Device()
{
    DX12Util::Init();

    u32 dxgiFactoryFlags = 0;

#ifdef _DEBUG
    {
        ComPtr<ID3D12Debug> debug = nullptr;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug))))
        {
            debug->EnableDebugLayer();
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
    }
#endif

    if (SUCCEEDED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&_Factory))))
    {
        ComPtr<IDXGIFactory6> factory6;
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

                if (SUCCEEDED(D3D12CreateDevice(_Adapter.Get(), D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&_Device))))
                {
                    break;
                }
            }
        }

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

                if (SUCCEEDED(D3D12CreateDevice(_Adapter.Get(), D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&_Device))))
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

void DX12Device::ReportLiveObjects()
{
#ifdef _DEBUG
    ComPtr<IDXGIDebug1> dxgiDebug;
    if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
    {
        dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
    }
#endif
}

DX12Device::~DX12Device()
{
    _Device.Reset();
    _Adapter.Reset();
    _Factory.Reset();

    ReportLiveObjects();
}

CommandQueue* DX12Device::CreateCommandQueue(const CommandType type)
{
    assert(_Device);
    D3D12_COMMAND_QUEUE_DESC queueDesc = {
        .Type = DX12Util::TranslateCommandType(type),
        .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE
    };
 
    ComPtr<ID3D12CommandQueue> commandQueue;
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
    ComPtr<ID3D12CommandAllocator> commandAlloc;
    if (SUCCEEDED(_Device->CreateCommandAllocator(DX12Util::TranslateCommandType(type), IID_PPV_ARGS(&commandAlloc))))
    {
        return new DX12CommandAllocator(type,commandAlloc);
    }

    assert(0);
    return nullptr;
}

SwapChain* DX12Device::CreateSwapChain(const SwapChain::Config& config,CommandQueue* queue, const std::any hwnd)
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

    ComPtr<IDXGISwapChain1> swapChain;
    assert(hwnd.has_value());
    const HWND Handle = std::any_cast<HWND>(hwnd);
    if (SUCCEEDED(_Factory->CreateSwapChainForHwnd(std::any_cast<ID3D12CommandQueue*>(queue->Handle()),
        Handle,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain)
    ))
    {
        _Factory->MakeWindowAssociation(Handle, DXGI_MWA_NO_ALT_ENTER);
        ComPtr<IDXGISwapChain3> swapChain3;
        assert(SUCCEEDED(swapChain.As<IDXGISwapChain3>(&swapChain3)));

        //RVT descriptor heap
        ComPtr<ID3D12DescriptorHeap> rvtHeap;
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
            ComPtr<ID3D12Resource> res;
            assert(SUCCEEDED(swapChain3->GetBuffer(n, IID_PPV_ARGS(&res))));
            _Device->CreateRenderTargetView(res.Get(), nullptr, rvtHandle);

       
            rtResources[n] = new DX12Resource(res);
            
            RenderTargetView::Config rc = {
                .Format = config.Format,
                .Dimension = ViewDimension::Texture2D,
            };
            rvts[n] = new DX12RenderTargetView(rtResources[n], rc, descriptorHeap, rvtHandle);
            rvtHandle.ptr += hc.Size;
        }

        return new DX12SwapChain(config,descriptorHeap,rtResources, rvts, swapChain3);
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