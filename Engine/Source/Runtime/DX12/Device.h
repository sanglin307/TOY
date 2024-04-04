#pragma once

class DX12Device : public RenderDevice
{
public :
	DX12_API virtual void Init() override;
	DX12_API virtual void Destroy() override;
	DX12_API virtual CommandQueue* CreateCommandQueue(const CommandType type) override;
	DX12_API virtual CommandAllocator* CreateCommandAllocator(const CommandType type) override;
	DX12_API virtual CommandList* CreateCommandList(CommandAllocator* allocator, const CommandType type) override;
	DX12_API virtual DescriptorHeap* CreateDescriptorHeap(DescriptorType type, u32 num, bool gpuVisible) override;
	DX12_API virtual SwapChain* CreateSwapChain(const SwapChain::Config& config, CommandQueue* queue, const std::any hwnd = nullptr) override;
	DX12_API virtual RootSignature* CreateRootSignature(const std::vector<ShaderObject*>& shaders) override;
	DX12_API virtual GraphicPipeline* CreateGraphicPipeline(const GraphicPipeline::Desc& desc) override;
	DX12_API virtual void InitPixelFormat_Platform() override;
	DX12_API virtual BufferResource* CreateBuffer(CommandList* commandList,u64 size, u8* initData, bool needCpuAccess = false, bool needAlignment = true) override;

private:
	void ReportLiveObjects();
	ComPtr<ID3DBlob> GenerateRootSignatureBlob(const std::vector<ShaderObject*>& shaders);

	DXGI_FORMAT TranslatePixelFormat(PixelFormat format);
	void TranslateGraphicPipeline(const GraphicPipeline::Desc& pso, D3D12_GRAPHICS_PIPELINE_STATE_DESC& dxPso);
	void TranslateBlendState(const BlendDesc& blend, D3D12_BLEND_DESC& dxBlend);
	void TranslateRasterizerState(const RasterizerDesc& rasterizer, D3D12_RASTERIZER_DESC& dxRasterizer);
	void TranslateDepthStencilState(const DepthStencilDesc& depthState, D3D12_DEPTH_STENCIL_DESC& dxDepthState);
	void TranslateInputLayout(const std::vector<InputLayoutDesc>& inputLayouts, std::vector<D3D12_INPUT_ELEMENT_DESC>& dxInputLayout);
	D3D12_BLEND TranslateBlendFactor(const BlendFactor factor);
	D3D12_BLEND_OP TranslateBlendOp(const BlendOp op);
	D3D12_LOGIC_OP TranslateLogicOp(const LogicOp op);
	D3D12_CULL_MODE TranslateCullMode(const CullMode mode);
	D3D12_COMPARISON_FUNC TranslateComparisonFunc(const ComparisonFunc func);
	D3D12_STENCIL_OP TranslateStencilOp(const StencilOp op);

	ComPtr<IDXGIFactory4> _Factory;
	ComPtr<IDXGIAdapter1> _Adapter;
	ComPtr<ID3D12Device5> _Device;
};


