#pragma once

class DX12RenderContext : public RenderContext
{
public:
	DX12RenderContext(u32 frameCount);
	virtual ~DX12RenderContext();

private:
	CommandQueue* _DirectCommandQueue = nullptr;
	CommandAllocator** _DirectCommandAllocator = nullptr;
	CommandList* _DirectCommandList = nullptr;

	CommandQueue* _ComputeCommandQueue = nullptr;
	CommandAllocator** _ComputeCommandAllocator = nullptr;
	CommandList* _ComputeCommandList = nullptr;

	CommandQueue* _CopyCommandQueue = nullptr;
	CommandAllocator** _CopyCommandAllocator = nullptr;
	CommandList* _CopyCommandList = nullptr;

	u32 _FrameCount;
};