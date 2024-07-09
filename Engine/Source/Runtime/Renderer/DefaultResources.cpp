#include "Private.h"

DefaultResource& DefaultResource::Instance()
{
	static DefaultResource Inst;
	return Inst;
}

void DefaultResource::Init(RenderDevice* device)
{
	float3 normalValue = { 0,0,1.f };
	RenderBuffer::Desc dn = {
		.Size = sizeof(float) * 3,
		.Stride = 0,
		.Usage = (u32)ResourceUsage::VertexBuffer,
		.CpuAccess = 0,
		.Alignment = true,
		.InitData = (u8*)&normalValue
	};
	_VertexBuffers[(u32)VertexAttribute::Normal] = device->CreateBuffer("DefaultVertexBuffer_Normal", dn);
	_Resources.insert(_VertexBuffers[(u32)VertexAttribute::Normal]);

	float3 tangentValue = { 0,1,0.f };
	RenderBuffer::Desc dt = {
		.Size = sizeof(float) * 3,
		.Stride = 0,
		.Usage = (u32)ResourceUsage::VertexBuffer,
		.CpuAccess = 0,
		.Alignment = true,
		.InitData = (u8*)&tangentValue
	};
	_VertexBuffers[(u32)VertexAttribute::Tangent] = device->CreateBuffer("DefaultVertexBuffer_Tangent", dt);
	_Resources.insert(_VertexBuffers[(u32)VertexAttribute::Tangent]);

	float4 colorValue = { 1.f, 1.f, 1.f, 1.f };
	RenderBuffer::Desc dc = {
		.Size = sizeof(float) * 4,
		.Stride = 0,
		.Usage = (u32)ResourceUsage::VertexBuffer,
		.CpuAccess = 0,
		.Alignment = true,
		.InitData = (u8*)&colorValue
	};
	_VertexBuffers[(u32)VertexAttribute::Color0] = _VertexBuffers[(u32)VertexAttribute::Color1] = _VertexBuffers[(u32)VertexAttribute::Color2] = device->CreateBuffer("DefaultVertexBuffer_Color", dc);
	_Resources.insert(_VertexBuffers[(u32)VertexAttribute::Color0]);

	float2 uvValue = {};
	RenderBuffer::Desc duv = {
		.Size = sizeof(float) * 2,
		.Stride = 0,
		.Usage = (u32)ResourceUsage::VertexBuffer,
		.CpuAccess = 0,
		.Alignment = true,
		.InitData = (u8*)&uvValue
	};
	RenderBuffer* uvBuffer = device->CreateBuffer("DefaultVertexBuffer_UV", duv);
	for (u32 i = (u32)VertexAttribute::UV0; i <= (u32)VertexAttribute::UV4; i++)
	{
		_VertexBuffers[i] = uvBuffer;
	}
	_Resources.insert(uvBuffer);

}

void DefaultResource::Destroy()
{
	for (RenderResource* res : _Resources)
	{
		delete res;
	}
}