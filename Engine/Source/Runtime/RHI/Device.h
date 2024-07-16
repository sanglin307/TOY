#pragma once

class RenderDevice
{
public:
	RHI_API RenderDevice();
	RHI_API virtual ~RenderDevice();

	virtual void WaitGPUIdle() = 0;

	virtual std::any Handle() { return nullptr; };

	virtual RenderContext* BeginFrame(Swapchain* viewport) = 0;
	virtual void EndFrame(RenderContext* ctx, Swapchain* viewport) = 0;

	virtual Swapchain* CreateSwapchain(const Swapchain::Desc& info) = 0;
	virtual CommandQueue* CreateCommandQueue(const CommandType type) = 0;
	virtual CommandAllocator* CreateCommandAllocator(const CommandType type) = 0;
	virtual RenderContext* CreateCommandContext(CommandAllocator* allocator, const CommandType type) = 0;
	virtual DescriptorHeap* CreateDescriptorHeap(const DescriptorHeap::Config& c) = 0;
	virtual RenderPipeline* CreateGraphicPipeline(const std::string& name,const GraphicPipeline::Desc& desc) = 0;
	virtual RenderPipeline* CreateComputePipeline(const std::string& name,const ComputePipeline::Desc& desc) = 0;
	virtual RenderBuffer* CreateBuffer(const std::string& name, const RenderBuffer::Desc& info) = 0;
	virtual RenderTexture* CreateTexture(const std::string& name, const RenderTexture::Desc& desc) = 0;
	virtual Sampler* CreateSampler(const Sampler::Desc& desc) = 0;
	virtual Fence* CreateFence(u64 initValue) = 0;

	RHI_API void CreateInputLayout(const ShaderResource* shader, InputSlotMapping slotMapping, InputLayout& inputLayout);

	RHI_API u8 GetPixelComponentSize(PixelFormat format);
	RHI_API u8 GetPixelComponentNum(PixelFormat format);
	RHI_API u8 GetPixelSize(PixelFormat format);
	RHI_API bool IsCompressedPixelFormat(PixelFormat format);
	RHI_API bool IsDepthPixelFormat(PixelFormat format);
	RHI_API bool IsStencilPixelFormat(PixelFormat format);
	RHI_API PixelFormat GetDepthShaderResourceFormat(PixelFormat format, bool useAsDepth);
	RHI_API PixelFormat GetDepthResourceFormat(PixelFormat format);
	RHI_API PixelFormat GetFormatByName(const std::string& name);

	RHI_API void InitPipelineCache();
	RHI_API RenderPipeline* LoadPipeline(const u64 hash);
	RHI_API RenderPipeline* LoadPipeline(const std::string& name);
	RHI_API ShaderResource* LoadShader(const ShaderCreateDesc& desc);

	RHI_API VertexAttribute TranslateSemanticToAttribute(const std::string& semanticName, u32 semanticIndex);

	virtual void OnResize(Swapchain* swapchain, u32 width, u32 height) = 0;

	void CommitCopyCommand() { _ContextManager->CommitCopyCommand(); }
	void GpuWaitCopyFinish() { _ContextManager->GpuWaitCopyFinish(); }

	void SetContextManager(ContextManager* manager)
	{
		_ContextManager = manager;
	}

	RHI_API void AddDelayDeleteResource(RenderResource* res, u64 copyFenceValue);
	RHI_API void CleanDelayDeleteResource();

	DescriptorHeap* GetCPUDescriptorHeap(DescriptorType type)
	{
		return _DescriptorManager->GetCPUHeap(type);
	}

	DescriptorHeap* GetGPUDescriptorHeap(DescriptorType type)
	{
		return _DescriptorManager->GetGPUHeap(type);
	}

protected:
	u64 _FrameNum = 0;
	std::vector<PixelFormatInfo> _Formats;
	std::unordered_map<std::string, PixelFormat> _FormatNameMap;
	std::list<DelayDeleteResource> _DelayDeleteResources;
	ContextManager* _ContextManager;
	DescriptorManager* _DescriptorManager;
	std::unordered_map<u64, RenderPipeline*> _PipelineCache;
	std::unordered_map<std::string, RenderPipeline*> _PipelineCacheByName;
	std::unordered_map<u64, ShaderResource*> _ShaderCache;
	std::unordered_map<u64, Sampler*> _SamplerCache;
	std::unordered_map<u64, RootSignature*> _RootSignatureCache;
};
