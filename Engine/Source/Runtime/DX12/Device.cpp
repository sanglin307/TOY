#include "Private.h"

#pragma comment(lib, "d3d12")   
#pragma comment(lib, "dxgi")  

#ifdef _DEBUG
#pragma comment(lib, "dxguid")   
#endif


D3D12_COMMAND_LIST_TYPE TranslateCommandType(const CommandType t)
{
    if (t == CommandType::Direct)
        return D3D12_COMMAND_LIST_TYPE_DIRECT;
    else if (t == CommandType::Bundle)
        return D3D12_COMMAND_LIST_TYPE_BUNDLE;
    else if (t == CommandType::Compute)
        return D3D12_COMMAND_LIST_TYPE_COMPUTE;
    else if (t == CommandType::Copy)
        return D3D12_COMMAND_LIST_TYPE_COPY;
    else
    {
        check(0);
        return D3D12_COMMAND_LIST_TYPE_NONE;
    }
}
 
DX12Device::DX12Device()
{
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
        check(0);
    }

    check(_Factory);
    check(_Adapter);
    check(_Device);

    InitPixelFormat_Platform();

    _ContextManager = new ContextManager(this);
    _DescriptorManager = new DescriptorManager(this);

    RootSignature::Desc rsDesc = {
        .RootCBVNum = 8,
        .RootSRVNum = 4,
        .RootUAVNum = 2,
        .TableCBVNum = 200,
        .TableSRVNum = 400,
        .TableUAVNum = 100,
        .TableSamplerNum = 100
    };
    _CachedRootSignature = LoadRootSignature(rsDesc);

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

void DX12Device::WaitGPUIdle()
{
    _ContextManager->WaitGPUIdle();
}

DX12Device::~DX12Device()
{
 
    for (auto iter = _RootSignatureCache.begin(); iter != _RootSignatureCache.end(); iter++)
    {
        delete iter->second;
    }

    for (auto iter = _PipelineCache.begin(); iter != _PipelineCache.end(); iter++)
    {
        delete iter->second;
    }

    for (auto iter = _ShaderCache.begin(); iter != _ShaderCache.end(); iter++)
    {
        delete iter->second;
    }


    RenderResourcePool::Instance().Destroy();

    delete _DescriptorManager;
    delete _ContextManager;

    _Device.Reset();
    _Adapter.Reset();
    _Factory.Reset();

    ReportLiveObjects();
}

Fence* DX12Device::CreateFence(u64 initValue)
{
    check(_Device);
 
    ComPtr<ID3D12Fence> fence;
    check(SUCCEEDED(_Device->CreateFence(initValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence))));

    HANDLE fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    check(fenceEvent);

    return new DX12Fence(fenceEvent, fence);

}

void DX12Device::OnResize(Swapchain* swapchain, u32 width, u32 height)
{
    WaitGPUIdle();

    DX12Swapchain* sc = static_cast<DX12Swapchain*>(swapchain);
    for (RenderResource* res : sc->_RenderTargets)
    {
        delete res;
    }
    sc->_RenderTargets.clear();
    sc->_Info.Width = width;
    sc->_Info.Height = height;

    DXGI_SWAP_CHAIN_DESC1 desc = {};
    sc->_Handle->GetDesc1(&desc);
    check(SUCCEEDED(sc->_Handle->ResizeBuffers(sc->_Info.FrameCount, sc->_Info.Width, sc->_Info.Height, TranslatePixelFormat(sc->_Info.Format), desc.Flags)));

    sc->_RenderTargets.resize(sc->_Info.FrameCount);
    DX12DescriptorHeap* rvtHeap = static_cast<DX12DescriptorHeap*>(_DescriptorManager->GetGPUHeap(DescriptorType::RVT));
    for (u32 n = 0; n < sc->_Info.FrameCount; n++)
    {
        ComPtr<ID3D12Resource> res;
        check(SUCCEEDED(sc->_Handle->GetBuffer(n, IID_PPV_ARGS(&res))));
        DescriptorAllocation da = rvtHeap->Allocate();
        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = rvtHeap->CPUHandle(da);
        _Device->CreateRenderTargetView(res.Get(), nullptr, cpuHandle);

        RenderTexture::Desc desc = {
             .Width = width,
             .Height = height,
             .DepthOrArraySize = 1,
             .Format = sc->_Info.Format,
             .Dimension = ResourceDimension::Texture2D
        };

        DX12RenderTexture* dx12Resource = new DX12RenderTexture("SwapChainTexture", this, desc, ResourceState::Present, res);
        dx12Resource->SetRTV(da, cpuHandle);

        sc->_RenderTargets[n] = dx12Resource;
    }
}

RenderContext* DX12Device::BeginFrame(Swapchain* sc)
{
    CommitCopyCommand();

    _FrameNum++;
    return _ContextManager->GetDirectContext(sc->GetCurrentFrameIndex());
}

void DX12Device::EndFrame(RenderContext* ctx, Swapchain* sc)
{
    // wait copy queue finish when excute direct queue.
    GpuWaitCopyFinish();

    RenderTexture* rt = sc->GetCurrentBackBuffer();
    ctx->Close(rt);

    CommandQueue* commandQueue = _ContextManager->GetDirectQueue();
    RenderContext* ctxs[] = { ctx };
    commandQueue->Excute(1,ctxs);

    u32 lastframeIndex = sc->GetCurrentFrameIndex();

    sc->Present(true);

    u32 nextFrameIndex = sc->GetCurrentFrameIndex();

    _ContextManager->SwitchToNextFrame(lastframeIndex, nextFrameIndex);

    // delete temp resources.
    CleanDelayDeleteResource();

}

CommandQueue* DX12Device::CreateCommandQueue(const CommandType type)
{
    check(_Device);
    D3D12_COMMAND_QUEUE_DESC queueDesc = {
        .Type = TranslateCommandType(type),
        .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE
    };
 
    ComPtr<ID3D12CommandQueue> commandQueue;
    if (SUCCEEDED(_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue))))
    {
        return new DX12CommandQueue(type,commandQueue);
    }
     
    check(0);
    return nullptr;
}

CommandAllocator* DX12Device::CreateCommandAllocator(const CommandType type)
{
    check(_Device);
    ComPtr<ID3D12CommandAllocator> commandAlloc;
    if (SUCCEEDED(_Device->CreateCommandAllocator(TranslateCommandType(type), IID_PPV_ARGS(&commandAlloc))))
    {
        return new DX12CommandAllocator(type,commandAlloc);
    }

    check(0);
    return nullptr;
}

RenderContext* DX12Device::CreateCommandContext(CommandAllocator* allocator, const CommandType type)
{
    check(_Device);
    ComPtr<ID3D12GraphicsCommandList> commandList;
    if (SUCCEEDED(_Device->CreateCommandList(0,TranslateCommandType(type), std::any_cast<ID3D12CommandAllocator*>(allocator->Handle()), nullptr, IID_PPV_ARGS(&commandList))))
    {
        commandList->Close();
        return new DX12CommandList(allocator, type, _ContextManager,this,commandList);
    }

    check(0);
    return nullptr;
}

Swapchain* DX12Device::CreateSwapchain(const Swapchain::Desc& info)
{
    check(_Factory);
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {
        .Width = info.Width,
        .Height = info.Height,
        .Format = TranslatePixelFormat(info.Format),
        .SampleDesc = {.Count = info.SampleCount, .Quality = info.SampleQuality},
        .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .BufferCount = info.FrameCount,
        .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
    };

    DX12DescriptorHeap* rvtHeap = static_cast<DX12DescriptorHeap*>(_DescriptorManager->GetGPUHeap(DescriptorType::RVT));
    CommandQueue* queue = _ContextManager->GetDirectQueue();

    ComPtr<IDXGISwapChain1> swapChain;
    check(info.HWND.has_value());
    const HWND Handle = std::any_cast<HWND>(info.HWND);
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
        check(SUCCEEDED(swapChain.As<IDXGISwapChain3>(&swapChain3)));

        std::vector<RenderTexture*> rtResources;
        rtResources.resize(info.FrameCount);
        for (u32 n = 0; n < info.FrameCount; n++)
        {
            ComPtr<ID3D12Resource> res;
            check(SUCCEEDED(swapChain3->GetBuffer(n, IID_PPV_ARGS(&res))));
            DescriptorAllocation da = rvtHeap->Allocate();
            D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = rvtHeap->CPUHandle(da);
            _Device->CreateRenderTargetView(res.Get(), nullptr, cpuHandle);
 
            RenderTexture::Desc desc = {
                 .Width = info.Width,
                 .Height = info.Height,
                 .DepthOrArraySize = 1,
                 .Format = info.Format,
                 .Dimension = ResourceDimension::Texture2D
            };

            DX12RenderTexture* dx12Resource = new DX12RenderTexture("SwapChainTexture",this, desc, ResourceState::Present, res);
            dx12Resource->SetRTV(da, cpuHandle);

            rtResources[n] = dx12Resource;
        }

        return new DX12Swapchain(info, rtResources, swapChain3);
    }


    check(0);
    return nullptr;
}

void DX12Device::CalculateRootSignatureDesc(std::array<ShaderResource*, (u32)ShaderProfile::MAX>& shaders,RootSignature::Desc& desc)
{
    desc = {};
    for (u32 i = 0; i < (u32)ShaderProfile::MAX; i++)
    {
        if (!shaders[i])
            continue;

        ShaderReflection* reflection = shaders[i]->GetReflection();
        if (reflection && reflection->BoundResources.size() > 0)
        {
            for (SRBoundResource& r : reflection->BoundResources)
            {
                if (r.Type == ShaderInputType::CBUFFER)
                {
                    if (r.BindSpace == RootSignature::cRootDescriptorSpace)
                    {
                        if (r.BindPoint + r.BindCount > desc.RootCBVNum)
                        {
                            desc.RootCBVNum = r.BindPoint + r.BindCount;
                        }
                    }
                    else if (r.BindSpace == RootSignature::cDescriptorTableSpace)
                    {
                        if (r.BindPoint + r.BindCount > desc.TableCBVNum)
                        {
                            desc.TableCBVNum = r.BindPoint + r.BindCount;
                        }
                    }
                    else
                        check(0);
                }
                else if ((r.Type == ShaderInputType::TBUFFER || r.Type == ShaderInputType::TEXTURE || r.Type == ShaderInputType::STRUCTURED ||
                    r.Type == ShaderInputType::BYTEADDRESS))
                {
                    if (r.BindSpace == RootSignature::cRootDescriptorSpace)
                    {
                        if (r.BindPoint + r.BindCount > desc.RootSRVNum)
                        {
                            desc.RootSRVNum = r.BindPoint + r.BindCount;
                        }
                    }
                    else if (r.BindSpace == RootSignature::cDescriptorTableSpace)
                    {
                        if (r.BindPoint + r.BindCount > desc.TableSRVNum)
                        {
                            desc.TableSRVNum = r.BindPoint + r.BindCount;
                        }
                    }
                    else
                        check(0);
                }
                else if ((r.Type == ShaderInputType::UAV_RWTYPED || r.Type == ShaderInputType::UAV_RWSTRUCTURED || r.Type == ShaderInputType::UAV_RWBYTEADDRESS ||
                    r.Type == ShaderInputType::UAV_RWSTRUCTURED_WITH_COUNTER || r.Type == ShaderInputType::UAV_FEEDBACKTEXTURE || r.Type == ShaderInputType::UAV_APPEND_STRUCTURED ||
                    r.Type == ShaderInputType::UAV_CONSUME_STRUCTURED))
                {
                    if (r.BindSpace == RootSignature::cRootDescriptorSpace)
                    {
                        if (r.BindPoint + r.BindCount > desc.RootUAVNum)
                        {
                            desc.RootUAVNum = r.BindPoint + r.BindCount;
                        }
                    }
                    else if (r.BindSpace == RootSignature::cDescriptorTableSpace)
                    {
                        if (r.BindPoint + r.BindCount > desc.TableUAVNum)
                        {
                            desc.TableUAVNum = r.BindPoint + r.BindCount;
                        }
                    }
                    else
                        check(0);
                }
                else if (r.Type == ShaderInputType::SAMPLER)
                {
                    if (r.BindSpace == RootSignature::cDescriptorTableSpace)
                    {
                        if (r.BindPoint + r.BindCount > desc.TableSamplerNum)
                        {
                            desc.TableSamplerNum = r.BindPoint + r.BindCount;
                        }
                    }
                    else
                        check(0);
                }
                else
                    check(0);
            }
        }
    }
}

ComPtr<ID3DBlob> DX12Device::GenerateRootSignatureBlob(const RootSignature::Desc& desc, std::vector<RootSignatureParamDesc>& paramDesc)
{
    // calculate root signature space limit
    constexpr static u32 cRootSignatureSizeLimit = 64;   // dword, from dx12 document.
    constexpr static u32 cRootTableSize = 1; // dword.
    constexpr static u32 cRootDescriptorsSize = 2; // dword.
    constexpr static u32 cRootConstantSize = 1;
    if (desc.RootConstant32Num * cRootConstantSize + (desc.RootCBVNum + desc.RootSRVNum + desc.RootUAVNum) * cRootDescriptorsSize + 4 * cRootTableSize > cRootSignatureSizeLimit)
    {
        check(0);
        return nullptr;
    }

    std::vector<D3D12_DESCRIPTOR_RANGE1> ranges;
    ranges.reserve(4); // table cbv,srv,uav,sampler.
    std::vector<D3D12_ROOT_PARAMETER1> param;

    param.push_back(D3D12_ROOT_PARAMETER1{
           .ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS,
           .Constants = {
                .ShaderRegister = 0,
                .RegisterSpace = RootSignature::cRootConstantSpace,
                .Num32BitValues = desc.RootConstant32Num
           },
           .ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL
        });

    paramDesc.push_back(RootSignatureParamDesc{
        .Type = ShaderBindType::RootConstant,
        .DescriptorNum = desc.RootConstant32Num
        });

    for (u32 i = 0; i < desc.RootCBVNum; i++)
    {
        param.push_back(D3D12_ROOT_PARAMETER1{
            .ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV,
            .Descriptor = {
                .ShaderRegister = i,
                .RegisterSpace = RootSignature::cRootDescriptorSpace,
                .Flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE
            },
            .ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL
            });

        paramDesc.push_back(RootSignatureParamDesc{
            .Type = ShaderBindType::RootCBV,
            .DescriptorNum = 1
            });
    }
    
    for (u32 i = 0; i < desc.RootSRVNum; i++)
    {
        param.push_back(D3D12_ROOT_PARAMETER1{
            .ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV,
            .Descriptor = {
                .ShaderRegister = i,
                .RegisterSpace = RootSignature::cRootDescriptorSpace,
                .Flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE
            },
            .ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL
            });

        paramDesc.push_back(RootSignatureParamDesc{
           .Type = ShaderBindType::RootSRV,
           .DescriptorNum = 1
            });
    }

    for (u32 i = 0; i < desc.RootUAVNum; i++)
    {
        param.push_back(D3D12_ROOT_PARAMETER1{
            .ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV,
            .Descriptor = {
                .ShaderRegister = i,
                .RegisterSpace = RootSignature::cRootDescriptorSpace,
                .Flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE
            },
            .ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL
            });

        paramDesc.push_back(RootSignatureParamDesc{
           .Type = ShaderBindType::RootUAV,
           .DescriptorNum = 1
            });
    }

    DescriptorHeap* heap = _DescriptorManager->GetGPUHeap(DescriptorType::CBV_SRV_UAV);

    if (desc.TableCBVNum > 0)
    {
        ranges.push_back(D3D12_DESCRIPTOR_RANGE1{
            .RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
            .NumDescriptors = desc.TableCBVNum,
            .BaseShaderRegister = 0,
            .RegisterSpace = RootSignature::cDescriptorTableSpace,
            .Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE,
            .OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND,
            });

        param.push_back(D3D12_ROOT_PARAMETER1{
            .ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
            .DescriptorTable = {
                .NumDescriptorRanges = 1,
                .pDescriptorRanges = &ranges[ranges.size() - 1],
                },
            .ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL
            });

        paramDesc.push_back(RootSignatureParamDesc{
           .Type = ShaderBindType::TableCBV,
           .DescriptorNum = desc.TableCBVNum,
           .Alloc = heap->AllocateBlock(desc.TableCBVNum)
            });
    }

    if (desc.TableSRVNum > 0)
    {
        ranges.push_back(D3D12_DESCRIPTOR_RANGE1{
            .RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
            .NumDescriptors = desc.TableSRVNum,
            .BaseShaderRegister = 0,
            .RegisterSpace = RootSignature::cDescriptorTableSpace,
            .Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE,
            .OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND,
            });

        param.push_back(D3D12_ROOT_PARAMETER1{
            .ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
            .DescriptorTable = {
                .NumDescriptorRanges = 1,
                .pDescriptorRanges = &ranges[ranges.size() - 1],
                },
            .ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL
            });

        paramDesc.push_back(RootSignatureParamDesc{
           .Type = ShaderBindType::TableSRV,
           .DescriptorNum = desc.TableSRVNum,
           .Alloc = heap->AllocateBlock(desc.TableSRVNum)
            });
    }

    if (desc.TableUAVNum > 0)
    {
        ranges.push_back(D3D12_DESCRIPTOR_RANGE1{
           .RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV,
           .NumDescriptors = desc.TableUAVNum,
           .BaseShaderRegister = 0,
           .RegisterSpace = RootSignature::cDescriptorTableSpace,
           .Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE,
           .OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND,
            });

        param.push_back(D3D12_ROOT_PARAMETER1{
            .ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
            .DescriptorTable = {
                .NumDescriptorRanges = 1,
                .pDescriptorRanges = &ranges[ranges.size() - 1],
                },
            .ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL
            });

        paramDesc.push_back(RootSignatureParamDesc{
           .Type = ShaderBindType::TableUAV,
           .DescriptorNum = desc.TableUAVNum,
           .Alloc = heap->AllocateBlock(desc.TableUAVNum)
            });
    }

    heap = _DescriptorManager->GetGPUHeap(DescriptorType::Sampler);
    if (desc.TableSamplerNum > 0)
    {
        ranges.push_back(D3D12_DESCRIPTOR_RANGE1{
           .RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER,
           .NumDescriptors = desc.TableSamplerNum,
           .BaseShaderRegister = 0,
           .RegisterSpace = RootSignature::cDescriptorTableSpace,
           .Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE,
           .OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND,
            });

        param.push_back(D3D12_ROOT_PARAMETER1{
            .ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
            .DescriptorTable = {
                .NumDescriptorRanges = 1,
                .pDescriptorRanges = &ranges[ranges.size() - 1],
                },
            .ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL
            });

        paramDesc.push_back(RootSignatureParamDesc{
           .Type = ShaderBindType::TableSampler,
           .DescriptorNum = desc.TableSamplerNum,
           .Alloc = heap->AllocateBlock(desc.TableSamplerNum)
            });
    }

    D3D12_VERSIONED_ROOT_SIGNATURE_DESC rsDesc = {
        .Version = D3D_ROOT_SIGNATURE_VERSION_1_1,
        .Desc_1_1 = {
           .NumParameters = (u32)param.size(),
           .pParameters = param.data(),
           .NumStaticSamplers = 0,
           .pStaticSamplers = nullptr,
           .Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
                    D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
                    D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
                    D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
                    D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED |
                    D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED,
        }
    };

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    if (FAILED(D3D12SerializeVersionedRootSignature(&rsDesc, signature.GetAddressOf(), error.GetAddressOf())))
    {
        std::string se((const char*)error->GetBufferPointer(), error->GetBufferSize());
        LOG_ERROR(RootSignature, std::format("D3D12SerializeVersionedRootSignature Failed:{}", se));
        return nullptr;
    }

    return signature;
     
}


// used for pso create.
D3D12_PRIMITIVE_TOPOLOGY_TYPE TranslatePrimitiveTopologyType(PrimitiveTopology topology)
{
    if (topology == PrimitiveTopology::Triangle)
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    else if (topology == PrimitiveTopology::Point)
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
    else if (topology == PrimitiveTopology::Line)
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    else
        check(0);
    
    return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
}

D3D12_BLEND DX12Device::TranslateBlendFactor(const BlendFactor factor)
{
    switch (factor)
    {
    case BlendFactor::Zero:
        return D3D12_BLEND_ZERO;
    case BlendFactor::One:
        return D3D12_BLEND_ONE;
    case BlendFactor::SrcColor:
        return D3D12_BLEND_SRC_COLOR;
    case BlendFactor::InvSrcColor:
        return D3D12_BLEND_INV_SRC_COLOR;
    case BlendFactor::SrcAlpha:
        return D3D12_BLEND_SRC_ALPHA;
    case BlendFactor::InvSrcAlpha:
        return D3D12_BLEND_INV_SRC_ALPHA;
    case BlendFactor::DestAlpha:
        return D3D12_BLEND_DEST_ALPHA;
    case BlendFactor::InvDestAlpha:
        return D3D12_BLEND_INV_DEST_ALPHA;
    case BlendFactor::DestColor:
        return D3D12_BLEND_DEST_COLOR;
    case BlendFactor::InvDestColor:
        return D3D12_BLEND_INV_DEST_COLOR;
    case BlendFactor::SrcAlphaSat:
        return D3D12_BLEND_SRC_ALPHA_SAT;
    case BlendFactor::BlendFactor:
        return D3D12_BLEND_BLEND_FACTOR;
    case BlendFactor::InvBlendFactor:
        return D3D12_BLEND_INV_BLEND_FACTOR;
    case BlendFactor::Src1Color:
        return D3D12_BLEND_SRC1_COLOR;
    case BlendFactor::InvSrc1Color:
        return D3D12_BLEND_INV_SRC1_COLOR;
    case BlendFactor::Src1Alpha:
        return D3D12_BLEND_SRC1_ALPHA;
    case BlendFactor::InvSrc1Alpha:
        return D3D12_BLEND_INV_SRC1_ALPHA;
    case BlendFactor::AlphaFactor:
        return D3D12_BLEND_ALPHA_FACTOR;
    case BlendFactor::InvAlphaFactor:
        return D3D12_BLEND_INV_ALPHA_FACTOR;
    default:
        check(0);
    }

    return D3D12_BLEND_ZERO;
}

D3D12_BLEND_OP DX12Device::TranslateBlendOp(const BlendOp op)
{
    switch (op)
    {
    case BlendOp::Add:
        return D3D12_BLEND_OP_ADD;
    case BlendOp::Subtract:
        return D3D12_BLEND_OP_SUBTRACT;
    case BlendOp::RevSubtract:
        return D3D12_BLEND_OP_REV_SUBTRACT;
    case BlendOp::Min:
        return D3D12_BLEND_OP_MIN;
    case BlendOp::MAX:
        return D3D12_BLEND_OP_MAX;
    default:
        check(0);
    }

    return D3D12_BLEND_OP_ADD;
}

D3D12_LOGIC_OP DX12Device::TranslateLogicOp(const LogicOp op)
{
    switch (op)
    {
    case LogicOp::Clear:
        return D3D12_LOGIC_OP_CLEAR;
    case LogicOp::Set:
        return D3D12_LOGIC_OP_SET;
    case LogicOp::Copy:
        return D3D12_LOGIC_OP_COPY;
    case LogicOp::CopyInverted:
        return D3D12_LOGIC_OP_COPY_INVERTED;
    case LogicOp::Noop:
        return D3D12_LOGIC_OP_NOOP;
    case LogicOp::Invert:
        return D3D12_LOGIC_OP_INVERT;
    case LogicOp::And:
        return D3D12_LOGIC_OP_AND;
    case LogicOp::Nand:
        return D3D12_LOGIC_OP_NAND;
    case LogicOp::Or:
        return D3D12_LOGIC_OP_OR;
    case LogicOp::Nor:
        return D3D12_LOGIC_OP_NOR;
    case LogicOp::Xor:
        return D3D12_LOGIC_OP_XOR;
    case LogicOp::Equiv:
        return D3D12_LOGIC_OP_EQUIV;
    case LogicOp::AndReverse:
        return D3D12_LOGIC_OP_AND_REVERSE;
    case LogicOp::AndInverted:
        return D3D12_LOGIC_OP_AND_INVERTED;
    case LogicOp::OrReverse:
        return D3D12_LOGIC_OP_OR_REVERSE;
    case LogicOp::OrInverted:
        return D3D12_LOGIC_OP_OR_INVERTED;
    default:
        check(0);
    }

    return D3D12_LOGIC_OP_NOOP;
}

D3D12_CULL_MODE DX12Device::TranslateCullMode(const CullMode mode)
{
    if (mode == CullMode::None)
        return D3D12_CULL_MODE_NONE;
    else if (mode == CullMode::Front)
        return D3D12_CULL_MODE_FRONT;
    else if (mode == CullMode::Back)
        return D3D12_CULL_MODE_BACK;
    else
        check(0);

    return D3D12_CULL_MODE_NONE;
}

D3D12_SRV_DIMENSION DX12Device::TranslateResourceViewDimension(const ResourceDimension rd, bool multipleSample)
{
    switch (rd)
    {
    case  ResourceDimension::Buffer:
        return D3D12_SRV_DIMENSION_BUFFER;
    case ResourceDimension::Texture1D:
        return D3D12_SRV_DIMENSION_TEXTURE1D;
    case ResourceDimension::Texture1DArray:
        return D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
    case ResourceDimension::Texture2D:
    {
        if (multipleSample)
            return D3D12_SRV_DIMENSION_TEXTURE2DMS;
        else
            return D3D12_SRV_DIMENSION_TEXTURE2D;
    }
    case ResourceDimension::Texture2DArray:
    {
        if (multipleSample)
            return D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;
        else
            return D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
    }
    case ResourceDimension::TextureCube:
        return D3D12_SRV_DIMENSION_TEXTURECUBE;
    case ResourceDimension::TextureCubeArray:
        return D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
    case ResourceDimension::Texture3D:
        return D3D12_SRV_DIMENSION_TEXTURE3D;
    default:
        return D3D12_SRV_DIMENSION_UNKNOWN;
    }

    return D3D12_SRV_DIMENSION_UNKNOWN;
}


D3D12_RESOURCE_DIMENSION DX12Device::TranslateResourceDimension(const ResourceDimension rd)
{
    switch (rd)
    {
    case  ResourceDimension::Buffer:
        return D3D12_RESOURCE_DIMENSION_BUFFER;
    case ResourceDimension::Texture1D:
    case ResourceDimension::Texture1DArray:
        return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
    case ResourceDimension::Texture2D:
    case ResourceDimension::Texture2DArray:
    case ResourceDimension::TextureCube:
    case ResourceDimension::TextureCubeArray:
        return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    case ResourceDimension::Texture3D:
        return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
    default:
        return D3D12_RESOURCE_DIMENSION_UNKNOWN;
    }
}

D3D12_COMPARISON_FUNC DX12Device::TranslateComparisonFunc(const ComparisonFunc func)
{
    switch (func)
    {
    case ComparisonFunc::None:
        return D3D12_COMPARISON_FUNC_NONE;
    case ComparisonFunc::Never:
        return D3D12_COMPARISON_FUNC_NEVER;
    case ComparisonFunc::Less:
        return D3D12_COMPARISON_FUNC_LESS;
    case ComparisonFunc::Equal:
        return D3D12_COMPARISON_FUNC_EQUAL;
    case ComparisonFunc::LessEqual:
        return D3D12_COMPARISON_FUNC_LESS_EQUAL;
    case ComparisonFunc::Greater:
        return D3D12_COMPARISON_FUNC_GREATER;
    case ComparisonFunc::NotEqual:
        return D3D12_COMPARISON_FUNC_NOT_EQUAL;
    case ComparisonFunc::GreaterEqual:
        return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
    case ComparisonFunc::Always:
        return D3D12_COMPARISON_FUNC_ALWAYS;
    default:
        check(0);
    }
    
    return D3D12_COMPARISON_FUNC_NONE;
}

D3D12_RESOURCE_STATES DX12Device::TranslateResourceState(const ResourceState state)
{
    if (state == ResourceState::Common)
        return D3D12_RESOURCE_STATE_COMMON;

    if (state == ResourceState::Predication)
        return D3D12_RESOURCE_STATE_PREDICATION;

    switch (state)
    {
    case ResourceState::VertexAndConstantBuffer:
        return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
    case ResourceState::IndexBuffer:
        return D3D12_RESOURCE_STATE_INDEX_BUFFER;
    case ResourceState::RenderTarget:
        return D3D12_RESOURCE_STATE_RENDER_TARGET;
    case ResourceState::UnorderedAccess:
        return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
    case ResourceState::DepthWrite:
        return D3D12_RESOURCE_STATE_DEPTH_WRITE;
    case ResourceState::DepthRead:
        return D3D12_RESOURCE_STATE_DEPTH_READ;
    case ResourceState::NonPixelShaderResource:
        return D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
    case ResourceState::PixelShaderResource:
        return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    case ResourceState::StreamOut:
        return D3D12_RESOURCE_STATE_STREAM_OUT;
    case ResourceState::IndirectArgument:
        return D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
    case ResourceState::CopyDest:
        return D3D12_RESOURCE_STATE_COPY_DEST;
    case ResourceState::CopySource:
        return D3D12_RESOURCE_STATE_COPY_SOURCE;
    case ResourceState::ResolveDest:
        return D3D12_RESOURCE_STATE_RESOLVE_DEST;
    case ResourceState::ResolveSource:
        return D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
    case ResourceState::RaytracingAccelerationStructure:
        return D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
    case ResourceState::ShadingRateSource:
        return D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE;
    case ResourceState::GenericRead:
        return D3D12_RESOURCE_STATE_GENERIC_READ;
    case ResourceState::AllShaderResource:
        return D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
    case ResourceState::Present:
        return D3D12_RESOURCE_STATE_PRESENT;
    }

    check(0);
    return D3D12_RESOURCE_STATE_COMMON;
}

D3D12_TEXTURE_ADDRESS_MODE DX12Device::TranslateTextureAddressMode(const TextureAddressMode am)
{
    switch (am)
    {
    case TextureAddressMode::Wrap:
        return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    case TextureAddressMode::Mirror:
        return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
    case TextureAddressMode::Clamp:
        return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    case TextureAddressMode::Border:
        return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    default:
        check(0);
    }

    return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
}

D3D12_FILTER DX12Device::TranslateSampleFilter(const SampleFilterMode minfilter, const SampleFilterMode magfilter, const SampleFilterMode mipfilter)
{
    if (minfilter == SampleFilterMode::Point && magfilter == SampleFilterMode::Point && mipfilter == SampleFilterMode::Point)
        return D3D12_FILTER_MIN_MAG_MIP_POINT;
    else if (minfilter == SampleFilterMode::Linear && magfilter == SampleFilterMode::Point && mipfilter == SampleFilterMode::Point)
        return D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;
    else if (minfilter == SampleFilterMode::Point && magfilter == SampleFilterMode::Point && mipfilter == SampleFilterMode::Linear)
        return D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
    else if (minfilter == SampleFilterMode::Point && magfilter == SampleFilterMode::Linear && mipfilter == SampleFilterMode::Point)
        return D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
    else if (minfilter == SampleFilterMode::Point && magfilter == SampleFilterMode::Linear && mipfilter == SampleFilterMode::Linear)
        return D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR;
    else if (minfilter == SampleFilterMode::Linear && magfilter == SampleFilterMode::Point && mipfilter == SampleFilterMode::Linear)
        return D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
    else if (minfilter == SampleFilterMode::Linear && magfilter == SampleFilterMode::Linear && mipfilter == SampleFilterMode::Point)
        return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
    else if (minfilter == SampleFilterMode::Linear && magfilter == SampleFilterMode::Linear && mipfilter == SampleFilterMode::Linear)
        return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    else if (minfilter == SampleFilterMode::Anisotropic && magfilter == SampleFilterMode::Anisotropic && mipfilter != SampleFilterMode::Anisotropic)
        return D3D12_FILTER_MIN_MAG_ANISOTROPIC_MIP_POINT;
    else if (minfilter == SampleFilterMode::Anisotropic && magfilter == SampleFilterMode::Anisotropic && mipfilter == SampleFilterMode::Anisotropic)
        return D3D12_FILTER_ANISOTROPIC;
    else
        check(0);

    return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
}

D3D_PRIMITIVE_TOPOLOGY DX12Device::TranslatePrimitiveTopology(const PrimitiveTopology topology)
{
    switch (topology)
    {
    case PrimitiveTopology::Point:
        return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
    case PrimitiveTopology::Line:
        return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
    case PrimitiveTopology::Triangle:
        return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    default:
        check(0);
    }

    return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

D3D12_STENCIL_OP DX12Device::TranslateStencilOp(const StencilOp op)
{
    switch (op)
    {
    case StencilOp::Keep:
        return D3D12_STENCIL_OP_KEEP;
    case StencilOp::Zero:
        return D3D12_STENCIL_OP_ZERO;
    case StencilOp::Replace:
        return D3D12_STENCIL_OP_REPLACE;
    case StencilOp::IncrSat:
        return D3D12_STENCIL_OP_INCR_SAT;
    case StencilOp::DecrSat:
        return D3D12_STENCIL_OP_DECR_SAT;
    case StencilOp::Invert:
        return D3D12_STENCIL_OP_INVERT;
    case StencilOp::Incr:
        return D3D12_STENCIL_OP_INCR;
    case StencilOp::Decr:
        return D3D12_STENCIL_OP_DECR;
    default:
        check(0);
    }

    return D3D12_STENCIL_OP_KEEP;
}

void DX12Device::TranslateBlendState(const BlendDesc& blend, D3D12_BLEND_DESC& dxBlend)
{
    dxBlend.AlphaToCoverageEnable = blend.AlphaToCoverageEnable;
    dxBlend.IndependentBlendEnable = blend.IndependentBlendEnable;
    for (i8 i = 0; i < MaxRenderTargetNumber; i++)
    {
        dxBlend.RenderTarget[i] = {
            .BlendEnable = blend.RenderTargets[i].Enable,
            .LogicOpEnable = blend.RenderTargets[i].LogicOpEnable,
            .SrcBlend = TranslateBlendFactor(blend.RenderTargets[i].Src),
            .DestBlend = TranslateBlendFactor(blend.RenderTargets[i].Dest),
            .BlendOp = TranslateBlendOp(blend.RenderTargets[i].Op),
            .SrcBlendAlpha = TranslateBlendFactor(blend.RenderTargets[i].SrcAlpha),
            .DestBlendAlpha = TranslateBlendFactor(blend.RenderTargets[i].DestAlpha),
            .BlendOpAlpha = TranslateBlendOp(blend.RenderTargets[i].AlphaOp),
            .LogicOp = TranslateLogicOp(blend.RenderTargets[i].LogicOp),
            .RenderTargetWriteMask = blend.RenderTargets[i].WriteMask
        };
    }
}

void DX12Device::TranslateRasterizerState(const RasterizerDesc& rasterizer, D3D12_RASTERIZER_DESC& dxRasterizer)
{
    dxRasterizer = {
        .FillMode = rasterizer.Fill == FillMode::Solid ? D3D12_FILL_MODE_SOLID : D3D12_FILL_MODE_WIREFRAME,
        .CullMode = TranslateCullMode(rasterizer.Cull),
        .FrontCounterClockwise = rasterizer.FrontCounterClockwise,
        .DepthBias = rasterizer.DepthBias,
        .DepthBiasClamp = rasterizer.DepthBiasClamp,
        .SlopeScaledDepthBias = rasterizer.SlopScaledDepthBias,
        .DepthClipEnable = rasterizer.DepthClip,
        .MultisampleEnable = rasterizer.Multisample,
        .AntialiasedLineEnable = rasterizer.LineAA,
        .ForcedSampleCount = rasterizer.SampleCount,
        .ConservativeRaster = rasterizer.ConservativeMode ? D3D12_CONSERVATIVE_RASTERIZATION_MODE_ON : D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
    };
}

void DX12Device::TranslateDepthStencilState(const DepthStencilDesc& depthState, D3D12_DEPTH_STENCIL_DESC& dxDepthState)
{
    dxDepthState = {
        .DepthEnable = depthState.DepthEnable,
        .DepthWriteMask = depthState.DepthWriteMask == DepthWriteMask::All ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO,
        .DepthFunc = TranslateComparisonFunc(depthState.DepthFunc),
        .StencilEnable = depthState.StencilEnable,
        .StencilReadMask = depthState.StencilReadMask,
        .StencilWriteMask = depthState.StencilWriteMask,
        .FrontFace = {
             .StencilFailOp = TranslateStencilOp(depthState.FrontStencilFail),
             .StencilDepthFailOp = TranslateStencilOp(depthState.FrontStencilDepthFail),
             .StencilPassOp = TranslateStencilOp(depthState.FrontStencilPass),
             .StencilFunc = TranslateComparisonFunc(depthState.FrontStencilFunc)
        },
        .BackFace = {
            .StencilFailOp = TranslateStencilOp(depthState.BackStencilFail),
            .StencilDepthFailOp = TranslateStencilOp(depthState.BackStencilDepthFail),
            .StencilPassOp = TranslateStencilOp(depthState.BackStencilPass),
            .StencilFunc = TranslateComparisonFunc(depthState.BackStencilFunc)
        }
    };
}

u64 DX12Device::GetTextureRequiredIntermediateSize(ID3D12Resource* resource, u32 firstSubresource, u32 subResourceNum)
{
    auto Desc = resource->GetDesc();
    u64 requiredSize = 0;

    _Device->GetCopyableFootprints(&Desc, firstSubresource, subResourceNum, 0, nullptr, nullptr, nullptr, &requiredSize);
    return requiredSize;
}

void DX12Device::TranslateInputLayout(const InputLayout& inputLayouts, std::vector<D3D12_INPUT_ELEMENT_DESC>& dxInputLayout)
{
    for (u32 i = 0; i < inputLayouts.Desc.size(); i++)
    {
        dxInputLayout.push_back({
            .SemanticName = inputLayouts.Desc[i].SemanticName.c_str(),
            .SemanticIndex = inputLayouts.Desc[i].SemanticIndex,
            .Format = TranslatePixelFormat(inputLayouts.Desc[i].Format),
            .InputSlot = inputLayouts.Desc[i].SlotIndex,
            .AlignedByteOffset = inputLayouts.Desc[i].SlotOffset,
            .InputSlotClass = inputLayouts.Desc[i].SlotClass == InputSlotClass::PerVertex ? D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA : D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,
            .InstanceDataStepRate = inputLayouts.Desc[i].InstanceStepRate
        });
    }
}

void DX12Device::TranslateGraphicPipeline(const GraphicPipeline::Desc& pso, std::array<ShaderResource*, (u32)ShaderProfile::MAX>& shaders, D3D12_GRAPHICS_PIPELINE_STATE_DESC& dxPso)
{
    dxPso = {
       .VS = {
            .pShaderBytecode = shaders[(u32)ShaderProfile::Vertex]->GetBlobData().Data,
            .BytecodeLength = shaders[(u32)ShaderProfile::Vertex]->GetBlobData().Size
       },
       .PS = {
            .pShaderBytecode = shaders[(u32)ShaderProfile::Pixel] ? shaders[(u32)ShaderProfile::Pixel]->GetBlobData().Data : nullptr,
            .BytecodeLength = shaders[(u32)ShaderProfile::Pixel] ? shaders[(u32)ShaderProfile::Pixel]->GetBlobData().Size : 0
        },
        .SampleMask = pso.SampleMask,
        .PrimitiveTopologyType = TranslatePrimitiveTopologyType(pso.Topology),
        .DSVFormat = TranslatePixelFormat(pso.DSVFormat),
        .SampleDesc = {
            .Count = pso.SampleCount,
            .Quality = pso.SampleQuality
        },
        .NodeMask = 0,
    };

    TranslateBlendState(pso.BlendState, dxPso.BlendState);
    TranslateRasterizerState(pso.RasterizerState, dxPso.RasterizerState);
    TranslateDepthStencilState(pso.DepthStencilState, dxPso.DepthStencilState);

    dxPso.NumRenderTargets = (UINT)pso.RVTFormats.size();
    for (u8 index = 0; index < dxPso.NumRenderTargets; ++index)
    {
        dxPso.RTVFormats[index] = TranslatePixelFormat(pso.RVTFormats[index]);
    }

}

RenderPipeline* DX12Device::CreateComputePipeline(const std::string& name, const ComputePipeline::Desc& desc)
{
    check(_Device);

    if (name.size() > 0 && desc.CS.Path.size() == 0)  // no shaders, find it using name only.
    {
        return LoadPipeline(name);
    }

    u64 hash = desc.HashResult();
    RenderPipeline* cache = LoadPipeline(hash);
    if (cache != nullptr)
    {
        return cache;
    }

    std::array<ShaderResource*, (u32)ShaderProfile::MAX> shaderRes = {};
    shaderRes[(u32)ShaderProfile::Compute] = LoadShader(desc.CS);

    // root signature
    RootSignature::Desc rd;
    CalculateRootSignatureDesc(shaderRes, rd);
    RootSignature* rs = LoadRootSignature(rd);
    check(rs);

    ComPtr<ID3D12PipelineState> pso;
    D3D12_COMPUTE_PIPELINE_STATE_DESC dxDesc = {
        .pRootSignature = std::any_cast<ID3D12RootSignature*>(rs->Handle()),
        .CS = {
            .pShaderBytecode = shaderRes[(u32)ShaderProfile::Compute]->GetBlobData().Data,
            .BytecodeLength = shaderRes[(u32)ShaderProfile::Compute]->GetBlobData().Size,
         },
        .Flags = D3D12_PIPELINE_STATE_FLAG_NONE
    };
 
    check(SUCCEEDED(_Device->CreateComputePipelineState(&dxDesc, IID_PPV_ARGS(&pso))));
    pso->SetName(PlatformUtils::UTF8ToUTF16(name).c_str());

    DX12ComputePipeline* pipeline = new DX12ComputePipeline(name, desc,pso);
    pipeline->AllocateParameters(rs, shaderRes);

    _PipelineCache[hash] = pipeline;

    if (!name.empty() && _PipelineCacheByName.find(name) == _PipelineCacheByName.end())
    {
        _PipelineCacheByName[name] = pipeline;
    }

    check(pipeline);
    return pipeline;
}

RenderPipeline* DX12Device::CreateGraphicPipeline(const std::string& name,const GraphicPipeline::Desc& desc)
{
    check(_Device);

    if (name.size() > 0 && desc.VS.Path.size() == 0)  // no shaders, find it using name only.
    {
        return LoadPipeline(name);
    }

    std::array<ShaderResource*, (u32)ShaderProfile::MAX> shaderRes = {};
    shaderRes[(u32)ShaderProfile::Vertex] = LoadShader(desc.VS);

    if (desc.VertexLayout.Desc.size() == 0)
    {
        CreateInputLayout(shaderRes[(u32)ShaderProfile::Vertex], InputSlotMapping::Seperated, (InputLayout&)desc.VertexLayout);
    }

    u64 hash = desc.HashResult();
    RenderPipeline* cache = LoadPipeline(hash);
    if (cache != nullptr)
    {
        return cache;
    }

    shaderRes[(u32)ShaderProfile::Pixel] = LoadShader(desc.PS);

    // root signature
    RootSignature::Desc rd;
    CalculateRootSignatureDesc(shaderRes, rd);
    RootSignature* rs = LoadRootSignature(rd);
    check(rs);

    ComPtr<ID3D12PipelineState> pso;
    D3D12_GRAPHICS_PIPELINE_STATE_DESC dxDesc = {};
    TranslateGraphicPipeline(desc, shaderRes, dxDesc);

    dxDesc.pRootSignature = std::any_cast<ID3D12RootSignature*>(rs->Handle());

    std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayouts;
    TranslateInputLayout(desc.VertexLayout, inputLayouts);
    dxDesc.InputLayout = {
            .pInputElementDescs = inputLayouts.data(),
            .NumElements = (UINT)inputLayouts.size()
    };
    check(SUCCEEDED(_Device->CreateGraphicsPipelineState(&dxDesc, IID_PPV_ARGS(&pso))));
    pso->SetName(PlatformUtils::UTF8ToUTF16(name).c_str());

    GraphicPipeline* pipeline = new DX12GraphicPipeline(name,desc,pso);
    pipeline->AllocateParameters(rs, shaderRes);
    
    _PipelineCache[hash] = pipeline;
    
    if (!name.empty() && _PipelineCacheByName.find(name) == _PipelineCacheByName.end())
    {
        _PipelineCacheByName[name] = pipeline;
    }

    return pipeline;
}

DXGI_FORMAT DX12Device::TranslatePixelFormat(PixelFormat format)
{
    return std::any_cast<DXGI_FORMAT>(_Formats[static_cast<u32>(format)].PlatformFormat);
}

void DX12Device::CopyDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE dest, D3D12_CPU_DESCRIPTOR_HANDLE src, D3D12_DESCRIPTOR_HEAP_TYPE  heapType)
{
    _Device->CopyDescriptorsSimple(1, dest, src, heapType);
}

RootSignature* DX12Device::LoadRootSignature(const RootSignature::Desc& rd)
{
    if (_CachedRootSignature != nullptr && _CachedRootSignature->Satisfy(rd))
        return _CachedRootSignature;

    u64 hash = rd.HashResult();
    auto iter = _RootSignatureCache.find(hash);
    if (iter != _RootSignatureCache.end())
    {
        return iter->second;
    }

    std::vector<RootSignatureParamDesc> paramDesc;
    ComPtr<ID3DBlob> signature = GenerateRootSignatureBlob(rd,paramDesc);
    ComPtr<ID3D12RootSignature> rs;
    check(SUCCEEDED(_Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rs))));
    std::string rsName = std::format("RCBV_{}_RSRV_{}_RUAV_{}_TCBV_{}_TSRV_{}_TUAV_{}_TS_{}", rd.RootCBVNum, rd.RootSRVNum, rd.RootUAVNum, rd.TableCBVNum, rd.TableSRVNum, rd.TableUAVNum, rd.TableSamplerNum);
    rs->SetName(PlatformUtils::UTF8ToUTF16(rsName).c_str());
    RootSignature* obj = new DX12RootSignature(rd,paramDesc,rs);

    _RootSignatureCache[hash] = obj;

    return obj;
}
 
void DX12Device::InitPixelFormat_Platform()
{
    check(_Formats.size() > 0);
    using enum PixelFormat;
    u32 index = 0;
#define DXGIFORMAT(F)  _Formats[index].PlatformFormat = DXGI_FORMAT_##F; ++index; _FormatNameMap[#F] = ##F;
    DXGIFORMAT(UNKNOWN)
        DXGIFORMAT(R32G32B32A32_TYPELESS)
        DXGIFORMAT(R32G32B32A32_FLOAT)
        DXGIFORMAT(R32G32B32A32_UINT)
        DXGIFORMAT(R32G32B32A32_SINT)
        DXGIFORMAT(R32G32B32_TYPELESS)
        DXGIFORMAT(R32G32B32_FLOAT)
        DXGIFORMAT(R32G32B32_UINT)
        DXGIFORMAT(R32G32B32_SINT)
        DXGIFORMAT(R16G16B16A16_TYPELESS)
        DXGIFORMAT(R16G16B16A16_FLOAT)
        DXGIFORMAT(R16G16B16A16_UNORM)
        DXGIFORMAT(R16G16B16A16_UINT)
        DXGIFORMAT(R16G16B16A16_SNORM)
        DXGIFORMAT(R16G16B16A16_SINT)
        DXGIFORMAT(R32G32_TYPELESS)
        DXGIFORMAT(R32G32_FLOAT)
        DXGIFORMAT(R32G32_UINT)
        DXGIFORMAT(R32G32_SINT)
        DXGIFORMAT(R32G8X24_TYPELESS)
        DXGIFORMAT(D32_FLOAT_S8X24_UINT)
        DXGIFORMAT(R32_FLOAT_X8X24_TYPELESS)
        DXGIFORMAT(X32_TYPELESS_G8X24_UINT)
        DXGIFORMAT(R10G10B10A2_TYPELESS)
        DXGIFORMAT(R10G10B10A2_UNORM)
        DXGIFORMAT(R10G10B10A2_UINT)
        DXGIFORMAT(R11G11B10_FLOAT)
        DXGIFORMAT(R8G8B8A8_TYPELESS)
        DXGIFORMAT(R8G8B8A8_UNORM)
        DXGIFORMAT(R8G8B8A8_UNORM_SRGB)
        DXGIFORMAT(R8G8B8A8_UINT)
        DXGIFORMAT(R8G8B8A8_SNORM)
        DXGIFORMAT(R8G8B8A8_SINT)
        DXGIFORMAT(R16G16_TYPELESS)
        DXGIFORMAT(R16G16_FLOAT)
        DXGIFORMAT(R16G16_UNORM)
        DXGIFORMAT(R16G16_UINT)
        DXGIFORMAT(R16G16_SNORM)
        DXGIFORMAT(R16G16_SINT)
        DXGIFORMAT(R32_TYPELESS)
        DXGIFORMAT(D32_FLOAT)
        DXGIFORMAT(R32_FLOAT)
        DXGIFORMAT(R32_UINT)
        DXGIFORMAT(R32_SINT)
        DXGIFORMAT(R24G8_TYPELESS)
        DXGIFORMAT(D24_UNORM_S8_UINT)
        DXGIFORMAT(R24_UNORM_X8_TYPELESS)
        DXGIFORMAT(X24_TYPELESS_G8_UINT)
        DXGIFORMAT(R8G8_TYPELESS)
        DXGIFORMAT(R8G8_UNORM)
        DXGIFORMAT(R8G8_UINT)
        DXGIFORMAT(R8G8_SNORM)
        DXGIFORMAT(R8G8_SINT)
        DXGIFORMAT(R16_TYPELESS)
        DXGIFORMAT(R16_FLOAT)
        DXGIFORMAT(D16_UNORM)
        DXGIFORMAT(R16_UNORM)
        DXGIFORMAT(R16_UINT)
        DXGIFORMAT(R16_SNORM)
        DXGIFORMAT(R16_SINT)
        DXGIFORMAT(R8_TYPELESS)
        DXGIFORMAT(R8_UNORM)
        DXGIFORMAT(R8_UINT)
        DXGIFORMAT(R8_SNORM)
        DXGIFORMAT(R8_SINT)
        DXGIFORMAT(A8_UNORM)
        DXGIFORMAT(R1_UNORM)
        DXGIFORMAT(R9G9B9E5_SHAREDEXP)
        DXGIFORMAT(R8G8_B8G8_UNORM)
        DXGIFORMAT(G8R8_G8B8_UNORM)
        DXGIFORMAT(BC1_TYPELESS)
        DXGIFORMAT(BC1_UNORM)
        DXGIFORMAT(BC1_UNORM_SRGB)
        DXGIFORMAT(BC2_TYPELESS)
        DXGIFORMAT(BC2_UNORM)
        DXGIFORMAT(BC2_UNORM_SRGB)
        DXGIFORMAT(BC3_TYPELESS)
        DXGIFORMAT(BC3_UNORM)
        DXGIFORMAT(BC3_UNORM_SRGB)
        DXGIFORMAT(BC4_TYPELESS)
        DXGIFORMAT(BC4_UNORM)
        DXGIFORMAT(BC4_SNORM)
        DXGIFORMAT(BC5_TYPELESS)
        DXGIFORMAT(BC5_UNORM)
        DXGIFORMAT(BC5_SNORM)
        DXGIFORMAT(B5G6R5_UNORM)
        DXGIFORMAT(B5G5R5A1_UNORM)
        DXGIFORMAT(B8G8R8A8_UNORM)
        DXGIFORMAT(B8G8R8X8_UNORM)
        DXGIFORMAT(R10G10B10_XR_BIAS_A2_UNORM)
        DXGIFORMAT(B8G8R8A8_TYPELESS)
        DXGIFORMAT(B8G8R8A8_UNORM_SRGB)
        DXGIFORMAT(B8G8R8X8_TYPELESS)
        DXGIFORMAT(B8G8R8X8_UNORM_SRGB)
        DXGIFORMAT(BC6H_TYPELESS)
        DXGIFORMAT(BC6H_UF16)
        DXGIFORMAT(BC6H_SF16)
        DXGIFORMAT(BC7_TYPELESS)
        DXGIFORMAT(BC7_UNORM)
        DXGIFORMAT(BC7_UNORM_SRGB)
#undef DXGIFORMAT
}

Sampler* DX12Device::CreateSampler(const Sampler::Desc& desc)
{
    u64 hash = desc.HashResult();
    auto iter = _SamplerCache.find(hash);
    if (iter != _SamplerCache.end())
        return iter->second;

    D3D12_SAMPLER_DESC samplerDesc = {
        .Filter = TranslateSampleFilter(desc.MinFilter,desc.MagFilter,desc.MipFilter),
        .AddressU = TranslateTextureAddressMode(desc.AddressU),
        .AddressV = TranslateTextureAddressMode(desc.AddressV),
        .AddressW = TranslateTextureAddressMode(desc.AddressW),
        .MipLODBias = desc.MipLODBias,
        .MaxAnisotropy = desc.MaxAnisotropy,
        .ComparisonFunc = TranslateComparisonFunc(desc.ComparisonFunc),
        .BorderColor = { desc.BorderColor[0], desc.BorderColor[1], desc.BorderColor[2], desc.BorderColor[3]},
        .MinLOD = desc.MinLOD,
        .MaxLOD = desc.MaxLOD
    };

    DX12DescriptorHeap* heap = static_cast<DX12DescriptorHeap*>(_DescriptorManager->GetCPUHeap(DescriptorType::Sampler));
    DescriptorAllocation descriptor = heap->Allocate();
    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = heap->CPUHandle(descriptor);
    _Device->CreateSampler(&samplerDesc, cpuHandle);

    Sampler* s = new DX12Sampler(desc,descriptor, cpuHandle);
    _SamplerCache[hash] = s;

    return s;

}