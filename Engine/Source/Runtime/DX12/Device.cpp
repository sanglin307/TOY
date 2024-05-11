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

    _FrameFence = CreateFence(0);
    _ContextManager = new ContextManager(this);
    _DescriptorManager = new DescriptorManager(this);

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
    u64 lastFenceValue = _ContextManager->GetMaxFenceValue();
    _ContextManager->GetDirectQueue()->Signal(_FrameFence, lastFenceValue);
    _FrameFence->CpuWait(lastFenceValue);
}

DX12Device::~DX12Device()
{
    delete _FrameFence;
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

RenderContext* DX12Device::BeginFrame(Swapchain* sc)
{
    RenderContext* context = _ContextManager->GetDirectContext(sc->GetCurrentFrameIndex());
    context->Reset();
    return context;
}

void DX12Device::EndFrame(RenderContext* ctx, Swapchain* sc)
{
    Texture2DResource* rt = sc->GetCurrentBackBuffer();
    ctx->Close(rt);
 
    CommandQueue* commandQueue = _ContextManager->GetDirectQueue();
    RenderContext* ctxs[] = { ctx };
    commandQueue->Excute(1,ctxs);

    u32 lastframeIndex = sc->GetCurrentFrameIndex();

    sc->Present(true);

    u32 nextFrameIndex = sc->GetCurrentFrameIndex();

    u64& lastFenceValue = _ContextManager->GetFenceValue(lastframeIndex);
    u64& nextFenceValue = _ContextManager->GetFenceValue(nextFrameIndex);

    commandQueue->Signal(_FrameFence, lastFenceValue);

    if (_FrameFence->GetCompletedValue() < nextFenceValue)
    {
        _FrameFence->CpuWait(nextFenceValue);
    }

    nextFenceValue = lastFenceValue + 1; // next frame fence value.
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
        return new DX12CommandList(allocator, type, commandList);
    }

    check(0);
    return nullptr;
}

Swapchain* DX12Device::CreateSwapchain(const Swapchain::CreateInfo& info)
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

    DescriptorHeap* rvtHeap = _DescriptorManager->GetRVTHeap();
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

        std::vector<Texture2DResource*> rtResources;
        rtResources.resize(info.FrameCount);
        D3D12_CPU_DESCRIPTOR_HANDLE rvtHandle = std::any_cast<D3D12_CPU_DESCRIPTOR_HANDLE>(rvtHeap->GetCPUDescriptorHandle(info.FrameCount));
        for (u32 n = 0; n < info.FrameCount; n++)
        {
            ComPtr<ID3D12Resource> res;
            check(SUCCEEDED(swapChain3->GetBuffer(n, IID_PPV_ARGS(&res))));
            _Device->CreateRenderTargetView(res.Get(), nullptr, rvtHandle);

            DX12Texture2DResource* dx12Resource = new DX12Texture2DResource(res);
            dx12Resource->SetRenderTargetView(rvtHandle);
            dx12Resource->Width = info.Width;
            dx12Resource->Height = info.Height;
            dx12Resource->Format = info.Format;
            dx12Resource->SetState(D3D12_RESOURCE_STATE_PRESENT);

            rtResources[n] = dx12Resource;
            rvtHandle.ptr += rvtHeap->GetStride();
        }

        return new DX12Swapchain(info, rtResources, swapChain3);
    }


    check(0);
    return nullptr;
}

ComPtr<ID3DBlob> DX12Device::GenerateRootSignatureBlob(const std::vector<ShaderObject*>& shaders)
{
    std::vector<D3D12_ROOT_PARAMETER1> param;
    std::vector<SRBoundResource*> CBV;
    std::vector<SRBoundResource*> SRV;
    std::vector<SRBoundResource*> UAV;
    std::vector<SRBoundResource*> Sampler;
    std::vector<D3D12_DESCRIPTOR_RANGE1> ranges;

    auto NonExist = [](std::vector<SRBoundResource*>& container, SRBoundResource* element) -> bool {
        return std::find_if(container.begin(), container.end(), [element](SRBoundResource* e) -> bool { return element->Name == e->Name; }) == container.end();
        };

    for (auto s : shaders)
    {
        if (s->Reflection && s->Reflection->BoundResources.size() > 0)
        {
            for (SRBoundResource& r : s->Reflection->BoundResources)
            {
                if(r.Type == ShaderInputType::CBUFFER && NonExist(CBV,&r))
                {
                    CBV.push_back(&r);
                }
                else if ((r.Type == ShaderInputType::TBUFFER || r.Type == ShaderInputType::TEXTURE || r.Type == ShaderInputType::STRUCTURED ||
                    r.Type == ShaderInputType::BYTEADDRESS) && NonExist(SRV, &r))
                {
                    SRV.push_back(&r);
                }
                else if ((r.Type == ShaderInputType::UAV_RWTYPED || r.Type == ShaderInputType::UAV_RWSTRUCTURED || r.Type == ShaderInputType::UAV_RWBYTEADDRESS ||
                    r.Type == ShaderInputType::UAV_RWSTRUCTURED_WITH_COUNTER || r.Type == ShaderInputType::UAV_FEEDBACKTEXTURE || r.Type == ShaderInputType::UAV_APPEND_STRUCTURED ||
                    r.Type == ShaderInputType::UAV_CONSUME_STRUCTURED ) && NonExist(UAV, &r))
                {
                    UAV.push_back(&r);
                }
                else if (r.Type == ShaderInputType::SAMPLER && NonExist(Sampler, &r))
                {
                    Sampler.push_back(&r);
                }
            }
        }
    }

    if (CBV.size() > 0)
    {
        ranges.push_back(D3D12_DESCRIPTOR_RANGE1{
            .RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
            .NumDescriptors = (u32)CBV.size(),
            .BaseShaderRegister = CBV[0]->BindPoint,
            .RegisterSpace = CBV[0]->BindSpace,
            .Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE,
            .OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND,
        });
    }

    if (SRV.size() > 0)
    {
        ranges.push_back(D3D12_DESCRIPTOR_RANGE1{
            .RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
            .NumDescriptors = (u32)SRV.size(),
            .BaseShaderRegister = SRV[0]->BindPoint,
            .RegisterSpace = SRV[0]->BindSpace,
            .Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE,
            .OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND,
            });
    }

    if (UAV.size() > 0)
    {
        ranges.push_back(D3D12_DESCRIPTOR_RANGE1{
            .RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV,
            .NumDescriptors = (u32)UAV.size(),
            .BaseShaderRegister = UAV[0]->BindPoint,
            .RegisterSpace = UAV[0]->BindSpace,
            .Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE,
            .OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND,
            });
    }

    if (Sampler.size() > 0)
    {
        ranges.push_back(D3D12_DESCRIPTOR_RANGE1{
            .RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER,
            .NumDescriptors = (u32)Sampler.size(),
            .BaseShaderRegister = Sampler[0]->BindPoint,
            .RegisterSpace = Sampler[0]->BindSpace,
            .Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE,
            .OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND,
            });
    }

    param.push_back(D3D12_ROOT_PARAMETER1{
        .ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
        .DescriptorTable = {
           .NumDescriptorRanges = (u32)ranges.size(),
           .pDescriptorRanges = ranges.data()
         },
        .ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL
        });

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

RootSignature* DX12Device::CreateRootSignature(const std::vector<ShaderObject*>& shaders)
{
    check(_Device);
 
    ComPtr<ID3DBlob> signature = GenerateRootSignatureBlob(shaders);

    ComPtr<ID3D12RootSignature> rs;
    check(SUCCEEDED(_Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rs))));
    
    return new DX12RootSignature(rs);
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE TranslatePrimitiveTopology(PrimitiveTopology topology)
{
    if (topology == PrimitiveTopology::Triangle)
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    else if (topology == PrimitiveTopology::Point)
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
    else if (topology == PrimitiveTopology::Line)
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    else if (topology == PrimitiveTopology::Patch)
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
    
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

void DX12Device::TranslateInputLayout(const std::vector<InputLayoutDesc>& inputLayouts, std::vector<D3D12_INPUT_ELEMENT_DESC>& dxInputLayout)
{
    for (u32 i = 0; i < inputLayouts.size(); i++)
    {
        dxInputLayout.push_back({
            .SemanticName = inputLayouts[i].SemanticName.c_str(),
            .SemanticIndex = inputLayouts[i].SemanticIndex,
            .Format = TranslatePixelFormat(inputLayouts[i].Format),
            .InputSlot = inputLayouts[i].SlotIndex,
            .AlignedByteOffset = inputLayouts[i].SlotOffset,
            .InputSlotClass = inputLayouts[i].SlotClass == InputSlotClass::PerVertex ? D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA : D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,
            .InstanceDataStepRate = inputLayouts[i].InstanceStepRate
        });
    }
}

void DX12Device::TranslateGraphicPipeline(const GraphicPipeline::Desc& pso, D3D12_GRAPHICS_PIPELINE_STATE_DESC& dxPso)
{
    dxPso = {
       .pRootSignature = std::any_cast<ID3D12RootSignature*>(pso.RootSignature->Handle()),
       .VS = {
            .pShaderBytecode = pso.VertexShader->BlobData,
            .BytecodeLength = pso.VertexShader->BlobSize
       },
       .PS = {
            .pShaderBytecode = pso.PixelShader ? pso.PixelShader->BlobData : nullptr,
            .BytecodeLength = pso.PixelShader ? pso.PixelShader->BlobSize : 0
        },
        .SampleMask = pso.SampleMask,
        .PrimitiveTopologyType = TranslatePrimitiveTopology(pso.Topology),
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

GraphicPipeline* DX12Device::CreateGraphicPipeline(const GraphicPipeline::Desc& desc)
{
    check(_Device);

    ComPtr<ID3D12PipelineState> pso;
    D3D12_GRAPHICS_PIPELINE_STATE_DESC dxDesc = {};
    TranslateGraphicPipeline(desc, dxDesc);

    std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayouts;
    TranslateInputLayout(desc.InputLayout, inputLayouts);
    dxDesc.InputLayout = {
            .pInputElementDescs = inputLayouts.data(),
            .NumElements = (UINT)inputLayouts.size()
    };
    check(SUCCEEDED(_Device->CreateGraphicsPipelineState(&dxDesc, IID_PPV_ARGS(&pso))));

    return new DX12GraphicPipeline(pso);
}

DXGI_FORMAT DX12Device::TranslatePixelFormat(PixelFormat format)
{
    return std::any_cast<DXGI_FORMAT>(_Formats[static_cast<u32>(format)].PlatformFormat);
}


void DX12Device::InitPixelFormat_Platform()
{
    check(_Formats.size() > 0);
    using enum PixelFormat;
    u32 index = 0;
#define DXGIFORMAT(F)  _Formats[index].PlatformFormat = DXGI_FORMAT_##F; ++index; 
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