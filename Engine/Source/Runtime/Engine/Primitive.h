#pragma once

class PrimitiveComponent
{
public:
	~PrimitiveComponent()
	{
		if (MeshData)
			delete MeshData;

		if (MaterialData)
			delete MaterialData;
	}

	Mesh* MeshData = nullptr;
	Material* MaterialData = nullptr;
	u64 PrimitiveId = 0;
};