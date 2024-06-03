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

struct ShaderCreateDesc
{
	ShaderProfile Profile;
	std::string  Path;
	std::string  Entry;
	std::vector<std::string> Macros;

	void HashUpdate(XXH64_state_t* state) const 
	{
		XXH64_update(state, &Profile, sizeof(Profile));
		XXH64_update(state, Path.c_str(), Path.length());
		XXH64_update(state, Entry.c_str(), Entry.length());
		for (u32 i = 0; i < Macros.size(); i++)
		{
			XXH64_update(state, Macros[i].c_str(), Macros[i].length());
		}
	}

	u64 HashResult() const
	{
		XXH64_state_t* const state = XXH64_createState();
		HashUpdate(state);
		XXH64_hash_t const hash = XXH64_digest(state);
		XXH64_freeState(state);
		return hash;
	}
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