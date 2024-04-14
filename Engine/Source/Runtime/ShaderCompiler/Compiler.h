#pragma once


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