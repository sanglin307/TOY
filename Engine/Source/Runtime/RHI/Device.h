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
	virtual RenderBuffer* CreateBuffer(RenderContext* ctx, const RenderBuffer::Desc& desc) = 0;
	virtual RenderTexture* CreateTexture(const RenderTexture::Desc& desc) = 0;
	virtual Fence* CreateFence(u64 initValue) = 0;

	void CreateInputLayout(const std::vector<ShaderResource*>& shaders, InputSlotMapping slotMapping, std::vector<InputLayoutDesc>& inputLayout);

	u8 GetPixelComponentSize(PixelFormat format);
	u8 GetPixelComponentNum(PixelFormat format);
	u8 GetPixelSize(PixelFormat format);
	bool IsCompressedPixelFormat(PixelFormat format);

	RHI_API void InitPipelineCache();
	RHI_API GraphicPipeline* LoadGraphicPipeline(const GraphicPipeline::Desc& desc);
	RHI_API ShaderResource* LoadShader(const ShaderCreateDesc& desc);

protected:
	std::vector<PixelFormatInfo> _Formats;

	Fence* _FrameFence;
	ContextManager* _ContextManager;
	DescriptorManager* _DescriptorManager;
	std::unordered_map<u64, GraphicPipeline*> _PipelineCache;
	std::unordered_map<u64, ShaderResource*> _ShaderCache;
};
