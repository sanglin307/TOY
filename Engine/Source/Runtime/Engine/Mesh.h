#pragma once

class MeshSegment
{
public:
	MeshSegment(PrimitiveTopology topology);
	~MeshSegment();
	void InsertAttribute(VertexAttribute attribute, const VertexData& data);
	void AddIndicesData(const VertexData& data);
	const std::array<VertexData, (u32)VertexAttribute::Max>& GetVertexData() const
	{
		return _Data;
	}

	const std::vector<u8>& GetPackedVertexData() const
	{
		return _PackedVertexData;
	}

	const VertexData& GetIndexData() const
	{
		return _IndicesData;
	}

	const std::vector<MeshletDesc>& GetMeshletDesc() const
	{
		return _Meshlets;
	}

	const std::vector<u32>& GetMeshletVertices() const
	{
		return _MeshletVertices;
	}

	const std::vector<u8>& GetMeshletTriangles() const
	{
		return _MeshletTriangles;
	}

	void SetMaterial(Material* mat);
	Material* GetMaterial()
	{
		return _Material;
	}

	void Build();

	constexpr static u32 cMeshletMaxVertices = 64;
	constexpr static u32 cMeshletMaxTriangles = 124;

	constexpr static u32 cVertexAttributeStride = 48; // should modify this if modify VertexAttribute format.

private:

	void BuildMeshlet();
	void BuildPackedData();

	std::array<VertexData, (u32)VertexAttribute::Max> _Data = {};
	std::vector<u8>  _PackedVertexData;
	std::vector<MeshletDesc> _Meshlets;
	std::vector<u32> _MeshletVertices;
	std::vector<u8>  _MeshletTriangles;
	VertexData  _IndicesData = {};
	Material*  _Material = nullptr;
	PrimitiveTopology  _Topology;
};

class MeshComponent : public PrimitiveComponent
{
public:
	MeshComponent(const std::string& name)
	{
		_Name = name;
	}
	virtual ~MeshComponent();
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

	void Build();

private:
	std::string _Name;
	std::vector<MeshSegment*> _Segments;
};