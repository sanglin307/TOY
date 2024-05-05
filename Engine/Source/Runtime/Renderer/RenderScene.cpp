#include "Private.h"

void RenderScene::Init()
{
 
	GraphicPipeline::Desc desc {
		.RootSignature = RD.CreateRootSignature(shaders),
	    .VertexShader = vs,
	    .PixelShader = ps
	};

	RootSignatureManager::Add(desc.RootSignature);

	RD.CreateInputLayout(shaders, InputSlotMapping::Interleaved, desc.InputLayout);
	GraphicPipeline* pso = RD.CreateGraphicPipeline(desc);
	PipelineManager::Add(pso);
 
 
	SceneConstantBuffer scene{};
	BufferResource* CB = RD.CreateBuffer(sizeof(SceneConstantBuffer), (u32)ResourceUsage::ConstBuffer,(u8*)&scene, 0, true);
	delete CB;
	
}

void RenderScene::Destroy()
{

}