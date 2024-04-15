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
		.RootSignature = RD.CreateRootSignature(shaders),
	    .VertexShader = vs,
	    .PixelShader = ps
	};

	RootSignatureManager::Add(desc.RootSignature);

	RD.CreateInputLayout(shaders, InputSlotMapping::Interleaved, desc.InputLayout);
	GraphicPipeline* pso = RD.CreateGraphicPipeline(desc);
	PipelineManager::Add(pso);
 

	f32 aspectRatio = RendererModule::Instance().Config().FrameWidth * 1.f / RendererModule::Instance().Config().FrameHeight;

	Vertex triangleVertices[] =
	{
		{ { 0.0f, 0.25f * aspectRatio, 0.0f }, { 1.0f, 0.0f, 0.0f } },
		{ { 0.25f, -0.25f * aspectRatio, 0.0f }, { 0.0f, 1.0f, 0.0f } },
		{ { -0.25f, -0.25f * aspectRatio, 0.0f }, { 0.0f, 0.0f, 1.0f } }
	};

	BufferResource* VB = RD.CreateBuffer(sizeof(triangleVertices), (u32)ResourceUsage::VertexBuffer, (u8*)triangleVertices,0, true);
	delete VB;

	SceneConstantBuffer scene{};
	BufferResource* CB = RD.CreateBuffer(sizeof(SceneConstantBuffer), (u32)ResourceUsage::ConstBuffer,(u8*)&scene, 0, true);
	delete CB;
	
}

void RenderScene::Destroy()
{

}