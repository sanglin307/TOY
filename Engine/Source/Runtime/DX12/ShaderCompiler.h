#pragma once

#include "Defines.h"
#include <dxcapi.h>
#include <d3d12shader.h>

enum class ShaderProfile
{
	VS = 0,
	PS,
	CS,
	GS,
	DS,
	HS,
	LIB
};


class ShaderCompiler
{
public:
	static void Init();
	static void Destroy();

	static void Compile(const std::string& shaderFileName, const std::string& entryPoint, const std::vector<std::string>& defines, bool debugInfo);

private:
	static IDxcUtils* _DXCUtils;
	static IDxcCompiler3* _DXCCompiler;

};