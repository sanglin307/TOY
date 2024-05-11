#pragma once

class RenderDevice
{
public:
	RHI_API RenderDevice();
	RHI_API virtual ~RenderDevice();

	virtual void WaitGPUIdle() = 0;

	virtual std::any Handle() { return nullptr; };

	virtual RenderContext* BeginFrame(RHIViewport* viewport) = 0;
	virtual void EndFrame(RenderContext* ctx, RHIViewport* viewport) = 0;

	virtual RHIViewport* CreateViewport(const RHIViewport::CreateInfo& info) = 0;
	virtual CommandQueue* CreateCommandQueue(const CommandType type) = 0;
	virtual CommandAllocator* CreateCommandAllocator(const CommandType type) = 0;
	virtual RenderContext* CreateCommandContext(CommandAllocator* allocator, const CommandType type) = 0;
	virtual DescriptorHeap* CreateDescriptorHeap(DescriptorType type, u32 num, bool gpuVisible) = 0;
	virtual RootSignature* CreateRootSignature(const std::vector<ShaderObject*>& shaders) = 0;
	virtual GraphicPipeline* CreateGraphicPipeline(const GraphicPipeline::Desc& desc) = 0;
	virtual BufferResource* CreateBuffer(RenderContext* ctx, u64 size, u32 Usage, u8* initData = nullptr, u32 stride = 0, bool needCpuAccess = false, bool needAlignment = true) = 0;
	virtual Fence* CreateFence(u64 initValue) = 0;

	void CreateInputLayout(const std::vector<ShaderObject*>& shaders, InputSlotMapping slotMapping, std::vector<InputLayoutDesc>& inputLayout);

	u8 GetPixelComponentSize(PixelFormat format);
	u8 GetPixelComponentNum(PixelFormat format);
	u8 GetPixelSize(PixelFormat format);
	bool IsCompressedPixelFormat(PixelFormat format);

protected:
	std::vector<PixelFormatInfo> _Formats;

	Fence* _FrameFence;
	ContextManager* _ContextManager;
	DescriptorManager* _DescriptorManager;
};
