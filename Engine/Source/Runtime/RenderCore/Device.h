#pragma once

class RenderDevice
{
public:
	static RenderDevice& Instance();
	RENDERCORE_API virtual void Init();
	virtual void Destroy() {};
	virtual std::any Handle() { return nullptr; };
	virtual CommandQueue* CreateCommandQueue(const CommandType type) = 0;
	virtual CommandAllocator* CreateCommandAllocator(const CommandType type) = 0;
	virtual CommandList* CreateCommandList(CommandAllocator* allocator, const CommandType type) = 0;
	virtual DescriptorHeap* CreateDescriptorHeap(DescriptorType type, u32 num, bool gpuVisible) = 0;
	virtual SwapChain* CreateSwapChain(const SwapChain::Config& config, CommandQueue* queue, DescriptorHeap* descriptorHeap, const std::any hwnd = nullptr) = 0;
	virtual RootSignature* CreateRootSignature(const std::vector<ShaderObject*>& shaders) = 0;
	virtual GraphicPipeline* CreateGraphicPipeline(const GraphicPipeline::Desc& desc) = 0;
	virtual void InitPixelFormat_Platform() = 0;
	virtual BufferResource* CreateBuffer(u64 size, u32 Usage, u8* initData = nullptr, u32 stride = 0, bool needCpuAccess = false, bool needAlignment = true) = 0;
	virtual Fence* CreateFence(u32 frameCount) = 0;

	RENDERCORE_API void CreateInputLayout(const std::vector<ShaderObject*>& shaders, InputSlotMapping slotMapping, std::vector<InputLayoutDesc>& inputLayout);

protected:
	void InitPixelFormat();
	u8 GetPixelComponentSize(PixelFormat format);
	u8 GetPixelComponentNum(PixelFormat format);
	u8 GetPixelSize(PixelFormat format);
    bool IsCompressedPixelFormat(PixelFormat format);

	std::vector<PixelFormatInfo> _Formats;
};
