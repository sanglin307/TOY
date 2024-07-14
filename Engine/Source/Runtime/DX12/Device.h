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
	virtual DescriptorHeap* CreateDescriptorHeap(const DescriptorHeap::Config& c) override;
	virtual Swapchain* CreateSwapchain(const Swapchain::Desc& desc) override;
	virtual RenderPipeline* CreateGraphicPipeline(const std::string& name,const GraphicPipeline::Desc& desc) override;
	virtual RenderPipeline* CreateComputePipeline(const std::string& name,const ComputePipeline::Desc& desc) override;
	virtual RenderBuffer* CreateBuffer(const std::string& name, const RenderBuffer::Desc& info) override;
	virtual RenderTexture* CreateTexture(const std::string& name,const RenderTexture::Desc& desc) override;
	virtual Sampler* CreateSampler(const Sampler::Desc& desc) override;
	virtual Fence* CreateFence(u64 initValue) override;

	virtual RenderContext* BeginFrame(Swapchain* viewport) override;
	virtual void EndFrame(RenderContext* ctx, Swapchain* viewport) override;

	virtual void OnResize(Swapchain* swapchain, u32 width, u32 height) override;

	DXGI_FORMAT TranslatePixelFormat(PixelFormat format);

	void CopyDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE dest, D3D12_CPU_DESCRIPTOR_HANDLE src, D3D12_DESCRIPTOR_HEAP_TYPE  heapType);
 
private:
	void ReportLiveObjects();
	 
    void InitPixelFormat_Platform();
	
	void TranslateGraphicPipeline(const GraphicPipeline::Desc& pso, std::array<ShaderResource*, (u32)ShaderProfile::MAX>& shaders, D3D12_GRAPHICS_PIPELINE_STATE_DESC& dxPso);
	void TranslateBlendState(const BlendDesc& blend, D3D12_BLEND_DESC& dxBlend);
	void TranslateRasterizerState(const RasterizerDesc& rasterizer, D3D12_RASTERIZER_DESC& dxRasterizer);
	void TranslateDepthStencilState(const DepthStencilDesc& depthState, D3D12_DEPTH_STENCIL_DESC& dxDepthState);
	void TranslateInputLayout(const InputLayout& inputLayouts, std::vector<D3D12_INPUT_ELEMENT_DESC>& dxInputLayout);

	u64 GetTextureRequiredIntermediateSize(ID3D12Resource* resource, u32 firstSubresource, u32 subResourceNum);
	void UpdateSubresource(ID3D12GraphicsCommandList* cmdList, ID3D12Resource* destResource, ID3D12Resource* tempResource, u64 tempResOffset, u32 firstSubresource, u32 NumSubresources, const D3D12_SUBRESOURCE_DATA* pSrcData);

	static D3D12_SRV_DIMENSION TranslateResourceViewDimension(const ResourceDimension rd, bool multipleSample = false);
	static D3D12_RESOURCE_DIMENSION TranslateResourceDimension(const ResourceDimension rd);
	static D3D12_BLEND TranslateBlendFactor(const BlendFactor factor);
	static D3D12_BLEND_OP TranslateBlendOp(const BlendOp op);
	static D3D12_LOGIC_OP TranslateLogicOp(const LogicOp op);
	static D3D12_CULL_MODE TranslateCullMode(const CullMode mode);
	static D3D12_COMPARISON_FUNC TranslateComparisonFunc(const ComparisonFunc func);
	static D3D12_STENCIL_OP TranslateStencilOp(const StencilOp op);
	static D3D12_RESOURCE_STATES TranslateResourceState(const ResourceState state);
	static D3D_PRIMITIVE_TOPOLOGY TranslatePrimitiveTopology(const PrimitiveTopology topology);
	static D3D12_FILTER TranslateSampleFilter(const SampleFilterMode minfilter, const SampleFilterMode magfilter, const SampleFilterMode mipfilter);
	static D3D12_TEXTURE_ADDRESS_MODE TranslateTextureAddressMode(const TextureAddressMode am);

	RootSignature* LoadRootSignature(const RootSignature::Desc& desc);
	void CalculateRootSignatureDesc(std::array<ShaderResource*, (u32)ShaderProfile::MAX>& shaders, RootSignature::Desc& desc);
	ComPtr<ID3DBlob> GenerateRootSignatureBlob(const RootSignature::Desc& desc, std::vector<RootSignatureParamDesc>& paramDesc);

	ComPtr<IDXGIFactory4> _Factory;
	ComPtr<IDXGIAdapter1> _Adapter;
	ComPtr<ID3D12Device5> _Device;

	RootSignature* _CachedRootSignature = nullptr; // if cache one satisfy our need ,use it.
 
};


