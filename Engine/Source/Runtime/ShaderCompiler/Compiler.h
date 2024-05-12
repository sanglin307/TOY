#pragma once


class ShaderCompiler
{
public:

	struct Args
	{
		ShaderProfile Profile;
		std::string Path;
		std::string EntryPoint;
		std::vector<std::string> Defines;
		bool Debug;
	};

	static SHADER_API ShaderResource* CompileHLSL(const Args& args);
};