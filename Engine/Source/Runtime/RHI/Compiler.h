#pragma once


class ShaderCompiler
{
public:
	RHI_API static void Init();
	static ShaderResource* CompileHLSL(const ShaderCreateDesc& args);
};