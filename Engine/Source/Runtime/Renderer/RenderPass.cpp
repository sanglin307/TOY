#include "Private.h"

void RenderPassTest::Init(RenderDevice* device)
{
	_Type = RenderPassType::Test;

	GraphicPipeline::Desc desc = {
		.Name = "TestPSO"
	};
	PSO = device->CreateGraphicPipeline(desc);

	RenderBuffer::Desc udesc = {
		.Size = sizeof(SceneConstantBuffer),
		.Name = "SceneConstantBuffer",
		.Usage = (u32)ResourceUsage::UniformBuffer,
		.CpuAccess = (u32)CpuAccessFlags::Write,
		.Alignment = true
	};
	UniformBuffer = device->CreateBuffer(udesc);
	PSO->BindParameter("SceneConstantBuffer", UniformBuffer);

}

void RenderPassTest::Render(RenderDevice* device, RenderContext* ctx)
{
	//update 
	const float translationSpeed = 0.005f;
	const float offsetBounds = 1.25f;

	UniformData.offset.x += translationSpeed;
	if (UniformData.offset.x > offsetBounds)
	{
		UniformData.offset.x = -offsetBounds;
	}
	UniformBuffer->UploadData((u8*)&UniformData, sizeof(UniformData));

	ctx->SetGraphicPipeline(PSO);
	for (auto c : _Commands)
	{
		std::vector<RenderBuffer*> buffers;
		for (auto v : c->VertexBuffers)
		{
			buffers.push_back(v.Buffer);
		}
		ctx->DrawInstanced((u32)buffers.size(), buffers.data());
	}
}

void RenderPassTest::AddPrimitive(PrimitiveSceneInfo* primitive)
{
	MeshCommand* command = new MeshCommand;
	command->VertexBuffers = primitive->VertexBuffers;
	command->IndexBuffer = primitive->IndexBuffer;
	command->PrimitiveId = primitive->PrimitiveId;
	command->PSO = PSO;
	_Commands.push_back(command);
}

void RenderPassTest::RemovePrimitive(PrimitiveSceneInfo* primitive)
{

}