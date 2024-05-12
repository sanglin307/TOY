#pragma once

class Mesh
{
public:
	virtual ~Mesh();
	static Mesh* Create(PrimitiveTopology topology);
	void InsertAttribute(VertexAttribute attribute, const VertexData& data);
private:
	std::vector<VertexData> _Data;
	PrimitiveTopology  _Topology;
};