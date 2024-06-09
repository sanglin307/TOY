#pragma once

class DX12Device final : public RenderDevice
{
	friend class DX12CommandList;
public :
	DX12Device();
	virtual ~DX12Device();
	virtual void WaitGPUIdle() override;
    virtual CommandQueue* CreateCommandQueue(const CommandType type) override;
	virtual CommandAllocator* CreateCommandAllocator(const CommandType type) override;
	virtual RenderContext* CreateCommandContext(CommandAllocator* allocator, const CommandType type) override;
	virtual DescriptorHeap* CreateDescriptorHeap(DescriptorType type, u32 num, bool gpuVisible) override;
	virtual Swapchain* CreateSwapchain(const Swapchain::Desc& desc) override;
	virtual GraphicPipeline* CreateGraphicPipeline(const GraphicPipeline::Desc& desc) override;
	virtual RenderBuffer* CreateBuffer(const RenderBuffer::Desc& desc) override;
	virtual RenderTexture* CreateTexture(const RenderTexture::Desc& desc) override;
	virtual Fence* CreateFence(u64 initValue) override;

	virtual RenderContext* BeginFrame(Swapchain* viewport) override;
	virtual void EndFrame(RenderContext* ctx, Swapchain* viewport) override;

	DXGI_FORMAT TranslatePixelFormat(PixelFormat format);

private:
	void ReportLiveObjects();
	ComPtr<ID3DBlob> GenerateRootSignatureBlob(std::array<ShaderResource*, (u32)ShaderProfile::MAX>& shaders);

    void InitPixelFormat_Platform();
	
	void TranslateGraphicPipeline(const GraphicPipeline::Desc& pso, std::array<ShaderResource*, (u32)ShaderProfile::MAX>& shaders, D3D12_GRAPHICS_PIPELINE_STATE_DESC& dxPso);
	void TranslateBlendState(const BlendDesc& blend, D3D12_BLEND_DESC& dxBlend);
	void TranslateRasterizerState(const RasterizerDesc& rasterizer, D3D12_RASTERIZER_DESC& dxRasterizer);
	void TranslateDepthStencilState(const DepthStencilDesc& depthState, D3D12_DEPTH_STENCIL_DESC& dxDepthState);
	void TranslateInputLayout(const InputLayout& inputLayouts, std::vector<D3D12_INPUT_ELEMENT_DESC>& dxInputLayout);
	
	static D3D12_BLEND TranslateBlendFactor(const BlendFactor factor);
	static D3D12_BLEND_OP TranslateBlendOp(const BlendOp op);
	static D3D12_LOGIC_OP TranslateLogicOp(const LogicOp op);
	static D3D12_CULL_MODE TranslateCullMode(const CullMode mode);
	static D3D12_COMPARISON_FUNC TranslateComparisonFunc(const ComparisonFunc func);
	static D3D12_STENCIL_OP TranslateStencilOp(const StencilOp op);
	static D3D12_RESOURCE_STATES TranslateResourceState(const ResourceState state);
	static D3D_PRIMITIVE_TOPOLOGY TranslatePrimitiveTopology(const PrimitiveTopology topology);

	ID3D12RootSignature* LoadRootSignature(u64 hash, std::array<ShaderResource*, (u32)ShaderProfile::MAX>& shaders);
	ID3D12RootSignature* LoadRootSignature(u64 hash);

	ComPtr<IDXGIFactory4> _Factory;
	ComPtr<IDXGIAdapter1> _Adapter;
	ComPtr<ID3D12Device5> _Device;

	std::unordered_map<u64, ComPtr<ID3D12RootSignature>> _RootSignatureCache;
 
};


