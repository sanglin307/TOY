#include "Private.h"

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

void MeshSegment::SetMaterial(Material* mat)
{
	_Material = mat;
}

void Mesh::AddSegment(MeshSegment* s)
{
	_Segments.push_back(s);
}



Mesh::~Mesh()
{
	for (MeshSegment* s : _Segments)
	{
		delete s;
	}
}