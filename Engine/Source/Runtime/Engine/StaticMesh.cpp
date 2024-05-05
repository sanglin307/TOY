#include "Private.h"

PostionVertexData* PostionVertexData::Create(Vector3f* data, u32 size)
{
	PostionVertexData* posData = new PostionVertexData;
	posData->_Data.resize(size);
	std::memcpy(posData->_Data.data(), data, sizeof(Vector3f) * size);
	return posData;
}

ColorVertexData* ColorVertexData::Create(Vector3f* data, u32 size)
{
	ColorVertexData* colorData = new ColorVertexData;
	colorData->_Data.resize(size);
	std::memcpy(colorData->_Data.data(), data, sizeof(Vector3f) * size);
	return colorData;
}

StaticMesh* StaticMesh::Create(VertexFormat* vertexFormat, PostionVertexData* positions, ColorVertexData* colors, Material* material)
{
	StaticMesh* mesh = new StaticMesh;
	mesh->_VertexFormat = vertexFormat;
	mesh->_Positions = positions;
	mesh->_Colors = colors;
	mesh->_MaterialResource = material;

	return mesh;
}

StaticMesh::~StaticMesh()
{
	delete _VertexFormat;
	_VertexFormat = nullptr;

}