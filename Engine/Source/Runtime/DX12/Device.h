#pragma once

class DX12Device final : public RenderDevice
{
public :
	DX12Device();
	virtual ~DX12Device();
	virtual void WaitGPUIdle() override;
    virtual CommandQueue* CreateCommandQueue(const CommandType type) override;
	virtual CommandAllocator* CreateCommandAllocator(const CommandType type) override;
	virtual RenderContext* CreateCommandContext(CommandAllocator* allocator, const CommandType type) override;
	virtual DescriptorHeap* CreateDescriptorHeap(DescriptorType type, u32 num, bool gpuVisible) override;
	virtual Swapchain* CreateSwapchain(const Swapchain::CreateInfo& info) override;
	virtual RootSignature* CreateRootSignature(const std::vector<ShaderResource*>& shaders) override;
	virtual GraphicPipeline* CreateGraphicPipeline(const GraphicPipeline::Desc& desc) override;
	virtual RenderBuffer* CreateBuffer(RenderContext* ctx, const RenderBuffer::CreateInfo& info) override;
	virtual Fence* CreateFence(u64 initValue) override;

	virtual RenderContext* BeginFrame(Swapchain* viewport) override;
	virtual void EndFrame(RenderContext* ctx, Swapchain* viewport) override;

	DXGI_FORMAT TranslatePixelFormat(PixelFormat format);

private:
	void ReportLiveObjects();
	ComPtr<ID3DBlob> GenerateRootSignatureBlob(const std::vector<ShaderResource*>& shaders);

    void InitPixelFormat_Platform();
	
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


