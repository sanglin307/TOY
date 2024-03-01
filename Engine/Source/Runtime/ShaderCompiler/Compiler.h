#pragma once

#include "Defines.h"



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
	Lib
};


class ShaderCompiler
{
public:

	struct CompileArgs
	{
		ShaderProfile Profile;
		std::string FileName;
		std::string EntryName;
		std::vector<std::string> Defines;
		bool Debug;
	};

	static SHADER_API bool CompileHLSL(const CompileArgs& args);

 
};