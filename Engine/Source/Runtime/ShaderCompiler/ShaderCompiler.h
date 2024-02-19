#pragma once

#include "Defines.h"



enum class ShaderProfile
{
	Vertex = 0,
	Pixel,
	Compute,
	Geometry,
	Demain,
	Hull,
	Lib
};


class SHADER_API ShaderCompiler
{
public:

	struct CompileArgs
	{
		std::string FileName;
		std::string EntryName;
		bool Debug;
	};

	static ShaderCompiler& Instance();

	void Init();
	void Destroy();

	void CompileHLSL(const CompileArgs& args);

private:
	IDxcUtils* _DXCUtils;
	IDxcCompiler3* _DXCCompiler;

};