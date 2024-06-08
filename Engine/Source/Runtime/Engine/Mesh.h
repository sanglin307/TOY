#pragma once

class Mesh
{
public:
	virtual ~Mesh();
	Mesh(PrimitiveTopology topology);
	void InsertAttribute(VertexAttribute attribute, const VertexData& data);
	const std::array<VertexData,(u32)VertexAttribute::Max>& GetVertexData() const
	{
		return _Data;
	}
private:
	std::array<VertexData,(u32)VertexAttribute::Max> _Data;
	PrimitiveTopology  _Topology;
};