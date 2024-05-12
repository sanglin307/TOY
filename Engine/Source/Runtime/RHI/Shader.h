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

struct ShaderBlobData
{
	u8* Data;
	u64 Size;
};

class ShaderResource
{
public:
	struct Desc
	{
		ShaderProfile Profile;
		std::map<std::string, std::string> Macro;
		std::string Hash;
		std::string DebugName;
		std::string Path;
		std::string EntryPoint;
		ShaderBlobData Blob;
		ShaderReflection* Reflection;
	};

	ShaderResource(const Desc& desc)
		:_Desc(desc)
	{
	}

	ShaderProfile GetProfile() { return _Desc.Profile; }
	ShaderReflection* GetReflection() { return _Desc.Reflection; }
	const std::string& GetDebugName() const { return _Desc.DebugName; }
	ShaderBlobData GetBlobData() { return _Desc.Blob; }
	const std::string& GetHash() const { return _Desc.Hash; }

	~ShaderResource()
	{
		if (_Desc.Blob.Data)
			delete _Desc.Blob.Data;

		if (_Desc.Reflection)
			delete _Desc.Reflection;
	}

private:
	Desc _Desc;
};