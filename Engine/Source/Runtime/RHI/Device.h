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
	virtual DescriptorHeap* CreateDescriptorHeap(DescriptorType type, u32 num, bool gpuVisible) = 0;
	virtual GraphicPipeline* CreateGraphicPipeline(const GraphicPipeline::Desc& desc) = 0;
	virtual RenderBuffer* CreateBuffer(const RenderBuffer::Desc& desc) = 0;
	virtual RenderTexture* CreateTexture(const RenderTexture::Desc& desc) = 0;
	virtual Fence* CreateFence(u64 initValue) = 0;

	RHI_API void CreateInputLayout(const ShaderResource* shader, InputSlotMapping slotMapping, InputLayout& inputLayout);

	u8 GetPixelComponentSize(PixelFormat format);
	u8 GetPixelComponentNum(PixelFormat format);
	u8 GetPixelSize(PixelFormat format);
	bool IsCompressedPixelFormat(PixelFormat format);

	RHI_API void InitPipelineCache();
	RHI_API GraphicPipeline* LoadGraphicPipeline(const GraphicPipeline::Desc& desc);
	RHI_API GraphicPipeline* LoadGraphicPipeline(const std::string& name);
	RHI_API ShaderResource* LoadShader(const ShaderCreateDesc& desc);

	void CommitCopyCommand() { _ContextManager->CommitCopyCommand(); }
	void GpuWaitCopyFinish() { _ContextManager->GpuWaitCopyFinish(); }

	void SetContextManager(ContextManager* manager)
	{
		_ContextManager = manager;
	}

	RHI_API void AddDelayDeleteResource(RenderResource* res, u32 delayFrame = 1);
	RHI_API void CleanDelayDeleteResource();

protected:
	u64 _FrameNum = 0;
	std::vector<PixelFormatInfo> _Formats;
	std::list<DelayDeleteResource> _DelayDeleteResources;
	ContextManager* _ContextManager;
	DescriptorManager* _DescriptorManager;
	std::unordered_map<u64, GraphicPipeline*> _PipelineCache;
	std::unordered_map<std::string, GraphicPipeline*> _PipelineCacheByName;
	std::unordered_map<u64, ShaderResource*> _ShaderCache;
};
