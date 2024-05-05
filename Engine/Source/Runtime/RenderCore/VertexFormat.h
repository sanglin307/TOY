#pragma once

class VertexFormat
{

};

class StaticMeshVF : public VertexFormat
{
public:
	struct Vertex
	{
		Vector3f Position;
		Vector3f Color;
	};
};