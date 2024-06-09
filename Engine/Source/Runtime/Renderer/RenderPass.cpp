#include "Private.h"

void RenderPassTest::Init(RenderDevice* device)
{
	_Type = RenderPassType::Test;

	GraphicPipeline::Desc desc = {
		.Name = "TestPSO"
	};
	PSO = device->CreateGraphicPipeline(desc);

}

void RenderPassTest::Render(RenderDevice* device, RenderContext* ctx)
{
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