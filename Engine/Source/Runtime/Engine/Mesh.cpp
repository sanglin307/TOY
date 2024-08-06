#include "Private.h"

MeshSegment::MeshSegment(PrimitiveTopology topology)
{
	_Topology = topology;
}

MeshSegment::~MeshSegment()
{
	for (VertexData& d : _Data)
	{
		if (d.Data)
			delete d.Data;
	}
}

void MeshSegment::InsertAttribute(VertexAttribute attribute, const VertexData& data)
{
	VertexData& d = _Data[(u32)attribute];
	check(d.Data == nullptr);

	d.Format = data.Format;
	d.Data = new u8[data.Size];
	d.Size = data.Size;
	std::memcpy(d.Data, data.Data, data.Size);
}

void MeshSegment::AddIndicesData(const VertexData& data)
{
	check(!_IndicesData.Data);
	_IndicesData.Format = data.Format;
	_IndicesData.Data = new u8[data.Size];
	_IndicesData.Size = data.Size;
	std::memcpy(_IndicesData.Data, data.Data, data.Size);
}

void MeshSegment::Build()
{
	BuildPackedData();
	BuildMeshlet();
}

void MeshSegment::BuildPackedData()
{
	u32 stride = 0;
	for (u32 i=(u32)VertexAttribute::Normal;i < (u32)VertexAttribute::Max;i++)
	{
		if (_Data[i].Data)
		{
			stride += _Data[i].GetStride();
		}
		else
		{
			if ((VertexAttribute)i == VertexAttribute::UV0)
			{
				stride += sizeof(float) * 2;
			}
			else
			{
				stride += sizeof(float) * 3;
			}
		}
	}

	check(cVertexAttributeStride == stride);

	u64 count = _Data[(u32)VertexAttribute::Position].GetCount();
	_PackedVertexData.resize(stride * count);

	u8* dest = _PackedVertexData.data();
	for (u64 i=0; i < count; i++)
	{
		if (_Data[(u32)VertexAttribute::Normal].Data)
		{
			u8* src = _Data[(u32)VertexAttribute::Normal].Data + i * _Data[(u32)VertexAttribute::Normal].GetStride();
			std::memcpy(dest, src, _Data[(u32)VertexAttribute::Normal].GetStride());
		}
		else
		{
			float3 normalValue = { 0,0,1.f };
			std::memcpy(dest, &normalValue, sizeof(float) * 3);
		}
		dest += sizeof(float) * 3;

		if (_Data[(u32)VertexAttribute::UV0].Data)
		{
			u8* src = _Data[(u32)VertexAttribute::UV0].Data + i * _Data[(u32)VertexAttribute::UV0].GetStride();
			std::memcpy(dest, src, _Data[(u32)VertexAttribute::UV0].GetStride());
		}
		else
		{
			float2 uvValue = { 0,0 };
			std::memcpy(dest, &uvValue, sizeof(float) * 2);
		}
		dest += sizeof(float) * 2;

		if (_Data[(u32)VertexAttribute::Tangent].Data)
		{
			u8* src = _Data[(u32)VertexAttribute::Tangent].Data + i * _Data[(u32)VertexAttribute::Tangent].GetStride();
			std::memcpy(dest, src, _Data[(u32)VertexAttribute::Tangent].GetStride());
		}
		else
		{
			float4 tangentValue = { 0,1,0.f,1 };
			std::memcpy(dest, &tangentValue, sizeof(float) * 4);
		}
		dest += sizeof(float) * 4;

		if (_Data[(u32)VertexAttribute::Color0].Data)
		{
			u8* src = _Data[(u32)VertexAttribute::Color0].Data + i * _Data[(u32)VertexAttribute::Color0].GetStride();
			std::memcpy(dest, src, _Data[(u32)VertexAttribute::Color0].GetStride());
		}
		else
		{
			float3 colorValue = { 1.f,1.f,1.f };
			std::memcpy(dest, &colorValue, sizeof(float) * 3);
		}
		dest += sizeof(float) * 3;
	}
}

void MeshSegment::BuildMeshlet()
{
	check(_IndicesData.Data);

	const float coneWeight = 0.f;

	u64 indexCount = _IndicesData.GetCount();
	u64 maxMeshlets = meshopt_buildMeshletsBound(indexCount, cMeshletMaxVertices, cMeshletMaxTriangles);
	std::vector<meshopt_Meshlet> meshlets(maxMeshlets);
	_MeshletVertices.resize(maxMeshlets * cMeshletMaxVertices);
	_MeshletTriangles.resize(maxMeshlets * cMeshletMaxTriangles * 3);

	u64 vertexCount = _Data[(u32)VertexAttribute::Position].GetCount(); 
	u64 meshletCount = meshopt_buildMeshlets(meshlets.data(), _MeshletVertices.data(), _MeshletTriangles.data(), _IndicesData.Data, indexCount,
		(const float*)_Data[(u32)VertexAttribute::Position].Data, vertexCount, _Data[(u32)VertexAttribute::Position].GetStride(), cMeshletMaxVertices, cMeshletMaxTriangles, coneWeight);

	const meshopt_Meshlet& last = meshlets[meshletCount - 1];
	_MeshletVertices.resize(last.vertex_offset + last.vertex_count);
	_MeshletTriangles.resize(last.triangle_offset + ((last.triangle_count * 3 + 3) & ~3));
	meshlets.resize(meshletCount);

	for (u32 i = 0; i < meshletCount; i++)
	{
		const meshopt_Meshlet& m = meshlets[i];
		meshopt_optimizeMeshlet(&_MeshletVertices[m.vertex_offset], &_MeshletTriangles[m.triangle_offset], m.triangle_count, m.vertex_count);

		meshopt_Bounds bounds = meshopt_computeMeshletBounds(&_MeshletVertices[m.vertex_offset], &_MeshletTriangles[m.triangle_offset],
			m.triangle_count, (const float*)_Data[(u32)VertexAttribute::Position].Data, _Data[(u32)VertexAttribute::Position].GetCount(), _Data[(u32)VertexAttribute::Position].GetStride());

		MeshletDesc& desc = _Meshlets.emplace_back();
		desc.TriangleCount = m.triangle_count;
		desc.TriangleOffset = m.triangle_offset;
		desc.VertexCount = m.vertex_count;
		desc.VertexOffset = m.vertex_offset;
		desc.CenterRadius = float4(bounds.center[0], bounds.center[1], bounds.center[2], bounds.radius);
	}
}

void MeshSegment::SetMaterial(Material* mat)
{
	_Material = mat;
}

void MeshComponent::AddSegment(MeshSegment* s)
{
	_Segments.push_back(s);
}

MeshComponent::~MeshComponent()
{
	for (MeshSegment* s : _Segments)
	{
		delete s;
	}
}

void MeshComponent::Build()
{
	for (MeshSegment* s : _Segments)
	{
		s->Build();
	}
}