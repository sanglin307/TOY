#pragma once

class DynamicRingBuffer
{
public:
	RHI_API DynamicRingBuffer(RenderDevice* device, u32 backbufferCount, u32 totalBytes);
	virtual ~DynamicRingBuffer();

	RHI_API u32 AllocateConstBuffer(u8* data, u32 size);
	RHI_API void BeginFrame();

	RHI_API RenderBuffer* GetResource()
	{
		return _Buffer;
	}

private:

	bool Alloc(u32 size, u32* pos);
	bool Free(u32 size);
	u32 GetTail();

	u32 _BackBufferCount;
	u32 _CurrentFrameAllocSize;
	std::vector<u32> _FrameAllocSize;

	u32 _TotalSize;
	u32 _AllocSize;
	u32 _Head;

	u8* _MappedData;
	RenderDevice* _Device;
	RenderBuffer* _Buffer;
};