#pragma once

class MeshSegment
{
public:
	MeshSegment(PrimitiveTopology topology)
	{
		_Topology = topology;
	}

	~MeshSegment();
	void InsertAttribute(VertexAttribute attribute, const VertexData& data);
	void AddIndicesData(const VertexData& data);
	const std::array<VertexData, (u32)VertexAttribute::Max>& GetVertexData() const
	{
		return _Data;
	}

	const VertexData& GetIndexData() const
	{
		return _IndicesData;
	}

	void SetMaterial(Material* mat);

private:
	std::array<VertexData, (u32)VertexAttribute::Max> _Data = {};
	VertexData  _IndicesData = {};
	Material*  _Material = nullptr;
	PrimitiveTopology  _Topology;
};

class Mesh : public PrimitiveComponent
{
public:
	Mesh(const std::string& name)
	{
		_Name = name;
	}
	virtual ~Mesh();
	void AddSegment(MeshSegment* segment);
	virtual PrimitiveType GetType() override
	{
		return PrimitiveType::Mesh;
	}

	const std::vector<MeshSegment*> GetSegments() const
	{
		return _Segments;
	}

	const std::string& GetName() const
	{
		return _Name;
	}

private:
	std::string _Name;
	std::vector<MeshSegment*> _Segments;
};