#include "Private.h"

void RenderScene::Init()
{
	std::vector<ShaderObject*> shaders;
	ShaderObject* vs = ShaderCompiler::CompileHLSL(ShaderCompiler::Args{
		.Profile = ShaderProfile::Vertex,
		.FileName = "shader.hlsl",
		.EntryName = "VSMain",
		.Debug = true
		});

	ShaderObject* ps = ShaderCompiler::CompileHLSL(ShaderCompiler::Args{
		.Profile = ShaderProfile::Pixel,
		.FileName = "shader.hlsl",
		.EntryName = "PSMain",
		.Debug = true
		});

	if (vs)
	{
		ShaderManager::Add(vs);
		shaders.push_back(vs);
	}

	if (ps)
	{
		ShaderManager::Add(ps);
		shaders.push_back(ps);
	}

	GraphicPipeline::Desc desc {
		.RootSignature = RHI.CreateRootSignature(shaders),
	    .VertexShader = vs,
	    .PixelShader = ps
	};

	RootSignatureManager::Add(desc.RootSignature);

	RHI.CreateInputLayout(shaders, InputSlotMapping::Interleaved, desc.InputLayout);
	GraphicPipeline* pso = RHI.CreateGraphicPipeline(desc);
	PipelineManager::Add(pso);

}

void RenderScene::Destroy()
{

}