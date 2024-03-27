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

class ShaderCompiler
{
public:

	struct Args
	{
		ShaderProfile Profile;
		std::string FileName;
		std::string EntryName;
		std::vector<std::string> Defines;
		bool Debug;
	};

	static SHADER_API ShaderObject* CompileHLSL(const Args& args); 
};