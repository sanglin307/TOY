#include "Private.h"

GVertexIndexBuffer& GVertexIndexBuffer::Instance()
{
	static GVertexIndexBuffer Inst;
	return Inst;
}

void GVertexIndexBuffer::Init(RenderDevice* device)
{
	_Device = device;
}

void GVertexIndexBuffer::Destroy()
{
	if (_PositionBuffer)
		delete _PositionBuffer;

	if (_VertexAttributesBuffer)
		delete _VertexAttributesBuffer;

	if (_IndexBuffer)
		delete _IndexBuffer;
}

void GVertexIndexBuffer::Create(const std::vector<RenderCluster*>& clusters)
{
	if (_PositionBuffer)
	{
		_Device->AddDelayDeleteResource(_PositionBuffer, DelayDeleteResourceType::Frame, _Device->GetCurrentFrameFenceValue());
		_PositionBuffer = nullptr;
	}

	if (_VertexAttributesBuffer)
	{
		_Device->AddDelayDeleteResource(_VertexAttributesBuffer, DelayDeleteResourceType::Frame, _Device->GetCurrentFrameFenceValue());
		_VertexAttributesBuffer = nullptr;
	}

	if (_IndexBuffer)
	{
		_Device->AddDelayDeleteResource(_IndexBuffer, DelayDeleteResourceType::Frame, _Device->GetCurrentFrameFenceValue());
		_IndexBuffer = nullptr;
	}

	u64 possize = 0;
	u64 attrisize = 0;
	u64 indexsize = 0;
	u32 vertexCount = 0;
	u32 indexCount = 0;
	for (RenderCluster* cluster : clusters)
	{
		_ClusterAlloc.push_back(ClusterAllocInfo{
        .VertexOffset = vertexCount,
		.IndexOffset = indexCount
		});

		vertexCount += (u32)cluster->PositionBuffer->GetElementCount();
		indexCount += (u32)cluster->IndexBuffer->GetElementCount();
		possize += cluster->PositionBuffer->GetSize();
		attrisize += cluster->CompactVertexAttributeBuffer->GetSize();
		indexsize += cluster->IndexBuffer->GetSize();
		check(cluster->IndexBuffer->GetStride() == 4); // TODO, use u32.
	}

	RenderBuffer::Desc pd = {
		.Size = possize,
		.Stride = sizeof(float) * 3,
		.Usage = (u32)ResourceUsage::ShaderResource,
		.Format = PixelFormat::R32G32B32_FLOAT,
		.CpuAccess = CpuAccessFlags::None,
		.Alignment = true
	};

	_PositionBuffer = _Device->CreateBuffer("GlobalPositionBuffer", pd);

	RenderBuffer::Desc vd = {
		.Size = attrisize,
		.Stride = MeshSegment::cVertexAttributeStride,
		.Usage = (u32)ResourceUsage::ShaderResource,
		.CpuAccess = CpuAccessFlags::None,
		.Alignment = true,
		.StructuredBuffer = true
	};

	_VertexAttributesBuffer = _Device->CreateBuffer("GlobalVertexAttributeBuffer", vd);

	RenderBuffer::Desc id = {
		.Size = indexsize,
		.Stride = sizeof(u32),
		.Usage = (u32)ResourceUsage::ShaderResource,
		.Format = PixelFormat::R32_UINT,
		.CpuAccess = CpuAccessFlags::None,
		.Alignment = true
	};

	_IndexBuffer = _Device->CreateBuffer("GlobalIndexBuffer", id);

	u64 copyFenceValue = 0;
	RenderContext* ctx = _Device->GetContextManager()->GetCopyContext(copyFenceValue);
	
	for(u32 i = 0;i < clusters.size();i++)
	{
		ClusterAllocInfo alloc = _ClusterAlloc[i];
		ctx->CopyBufferRegion(_PositionBuffer, alloc.VertexOffset * _PositionBuffer->GetStride(), clusters[i]->PositionBuffer, 0, clusters[i]->PositionBuffer->GetSize());
		ctx->CopyBufferRegion(_VertexAttributesBuffer, alloc.VertexOffset * _VertexAttributesBuffer->GetStride(), clusters[i]->CompactVertexAttributeBuffer, 0, clusters[i]->CompactVertexAttributeBuffer->GetSize());
		ctx->CopyBufferRegion(_IndexBuffer, alloc.IndexOffset * _IndexBuffer->GetStride(), clusters[i]->IndexBuffer, 0, clusters[i]->IndexBuffer->GetSize());
	}
}