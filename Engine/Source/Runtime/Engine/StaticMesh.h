#pragma once

class PostionVertexData
{
public:
	static PostionVertexData* Create(Vector3f* data, u32 size);
private:
	std::vector<Vector3f> _Data;
};

class ColorVertexData
{
public:
	static ColorVertexData* Create(Vector3f* data, u32 size);
private:
	std::vector<Vector3f> _Data;
};

class StaticMesh
{
public:
	virtual ~StaticMesh();
	static StaticMesh* Create(VertexFormat* vertexFormat, PostionVertexData* positions, ColorVertexData* colors, Material* material);
private:
	VertexFormat* _VertexFormat;
	PostionVertexData* _Positions;
	ColorVertexData*   _Colors;
	Material* _MaterialResource;
};