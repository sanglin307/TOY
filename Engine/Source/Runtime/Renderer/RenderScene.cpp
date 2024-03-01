#include "RenderScene.h"
#include "../ShaderCompiler/Compiler.h"

void RenderScene::Init()
{
	ShaderCompiler::CompileArgs args = {
		.Profile = ShaderProfile::Vertex,
		.FileName = "shader.hlsl",
		.EntryName = "VSMain",
		.Debug = true
	};
	ShaderCompiler::CompileHLSL(args);
}

void RenderScene::Destroy()
{

}