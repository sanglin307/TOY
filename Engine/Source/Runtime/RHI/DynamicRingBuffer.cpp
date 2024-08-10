#include "Private.h"

DynamicRingBuffer::DynamicRingBuffer(RenderDevice* device, u32 backbufferCount, u32 totalBytes)
{
    _TotalSize = (totalBytes + 255) & ~255;
    _AllocSize = 0;
    _Head = 0;

    _BackBufferCount = backbufferCount;
    _CurrentFrameAllocSize = 0;
    _FrameAllocSize.resize(_BackBufferCount, 0);

	_Device = device;
    RenderBuffer::Desc desc = {
        .Size = _TotalSize,
        .Format = PixelFormat::UNKNOWN,
        .CpuAccess = CpuAccessFlags::Write,
        .Alignment = false
    };

    _Buffer = _Device->CreateBuffer("DynamicRingBuffer", desc);
    _MappedData = _Buffer->GetMappingData();
}

u32 DynamicRingBuffer::GetTail()
{
    return (_Head + _AllocSize) % _TotalSize;
}

DynamicRingBuffer::~DynamicRingBuffer()
{
    delete _Buffer;
}

bool DynamicRingBuffer::Free(u32 size)
{
    if (size == 0)
        return true;

    if (_AllocSize >= size)
    {
        _Head = (_Head + size) % _TotalSize;
        _AllocSize -= size;
        return true;
    }

    check(0);
    return false;
}

bool DynamicRingBuffer::Alloc(u32 size, u32 *pos)
{
    u32 tail = GetTail();
    if ((tail + size) > _TotalSize)
    {
        u32 remain = _TotalSize - tail;
        _CurrentFrameAllocSize += remain; // skip the remain space.
        _AllocSize += remain;
    }

    if (_AllocSize + size <= _TotalSize)
    {
        *pos = GetTail();
        _AllocSize += size;
        _CurrentFrameAllocSize += size;
        return true;
    }

    check(0);
    return false;
}

u32 DynamicRingBuffer::AllocateConstBuffer(u8* data, u32 size)
{
    u32 copysize = size;
    size = (size + 255) & ~255;
    u32 pos = 0;
    if (!Alloc(size, &pos))
    {
        LOG_FATAL(DynamicRingBuffer, "Out of memory!!");
    }

    if (data)
    {
        u8* dst = _MappedData + pos;
        std::memcpy(dst, data, copysize);
    }

    return pos;
}

void DynamicRingBuffer::BeginFrame()
{
    u32 frameIndex = _Device->GetCurrentFrameIndex();
    _FrameAllocSize[frameIndex] = _CurrentFrameAllocSize;
    _CurrentFrameAllocSize = 0;

    // oldest frame free.
    frameIndex = (frameIndex + 1) % _BackBufferCount;
    Free(_FrameAllocSize[frameIndex]);
}

