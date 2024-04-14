#pragma once

enum class ShaderProfile
{
	Vertex = 0,
	Pixel,
	Compute,
	Geometry,
	Domain,
	Hull,
	Mesh,
	Amplification,
	Lib,
	MAX
};

struct ShaderObject
{
	~ShaderObject()
	{
		if (BlobData)
			delete BlobData;

		if (Reflection)
			delete Reflection;
	}

	ShaderProfile Profile;
	std::string Hash;
	std::string DebugName;
	u8* BlobData = nullptr;
	u64 BlobSize;
	ShaderReflection* Reflection = nullptr;
};