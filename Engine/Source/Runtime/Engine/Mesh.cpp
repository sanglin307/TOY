#include "Private.h"

Mesh::Mesh(PrimitiveTopology topology)
{
	_Topology = topology;
	_Data = {};
}

Mesh::~Mesh()
{
	for (VertexData& d : _Data)
	{
		if (d.Data)
			delete d.Data;
	}

}

void Mesh::InsertAttribute(VertexAttribute attribute, const VertexData& data)
{
	VertexData& d = _Data[(u32)attribute];
	check(d.Data == nullptr);

	d.Format = data.Format;
	d.Data = new u8[data.Size];
	d.Size = data.Size;
	std::memcpy(d.Data, data.Data, data.Size);
}