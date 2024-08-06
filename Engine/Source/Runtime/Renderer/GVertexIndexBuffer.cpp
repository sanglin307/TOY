#include "Private.h"

GVertexIndexBuffer& GVertexIndexBuffer::Instance()
{
	static GVertexIndexBuffer Inst;
	return Inst;
}

void GVertexIndexBuffer::Init(RenderDevice* device)
{
	_Device = device;
	_AllocCount = 100; // pre alloc.
	std::vector<RenderCluster*> temp;
	Create(temp);
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
	bool recreate = !_PositionBuffer;
	if (clusters.size() > _AllocCount)
	{
		_AllocCount = clusters.size();
		recreate = true;
	}

	if (recreate)
	{
		if (_PositionBuffer)
			delete _PositionBuffer;

		if (_VertexAttributesBuffer)
			delete _VertexAttributesBuffer;

		if (_IndexBuffer)
			delete _IndexBuffer;

		RenderBuffer::Desc pd = {
			.Size = _AllocCount * sizeof(float) * 3,
			.Stride = sizeof(float) * 3,
			.Usage = (u32)ResourceUsage::ShaderResource,
			.CpuAccess = CpuAccessFlags::None,
			.Alignment = true
		};

		_PositionBuffer = _Device->CreateBuffer("GlobalPositionBuffer", pd);

		RenderBuffer::Desc vd = {
			.Size = _AllocCount * MeshSegment::cVertexAttributeStride,
			.Stride = MeshSegment::cVertexAttributeStride,
			.Usage = (u32)ResourceUsage::ShaderResource,
			.CpuAccess = CpuAccessFlags::None,
			.Alignment = true
		};

		_VertexAttributesBuffer = _Device->CreateBuffer("GlobalVertexAttributeBuffer", vd);

		RenderBuffer::Desc id = {
			.Size = _AllocCount * sizeof(u16),
			.Stride = sizeof(u16),
			.Usage = (u32)ResourceUsage::ShaderResource,
			.CpuAccess = CpuAccessFlags::None,
			.Alignment = true
		};

		_IndexBuffer = _Device->CreateBuffer("GlobalIndexBuffer", id);
	}
}