#include "Private.h"

void RenderPassTest::Init(RenderDevice* device)
{
	_Type = RenderPassType::Test;

	/*GraphicPipeline::Desc desc = {
		.Name = "TestPSO",
		ShaderCreateDesc VS;
		ShaderCreateDesc PS;
		BlendDesc       BlendState;
		u32             SampleMask = 0xffffffff;
		RasterizerDesc  RasterizerState;
		DepthStencilDesc DepthStencilState;
		PrimitiveTopology Topology = PrimitiveTopology::Triangle;
		std::vector<PixelFormat> RVTFormats = { PixelFormat::R8G8B8A8_UNORM };
		PixelFormat DSVFormat = PixelFormat::D32_FLOAT_S8X24_UINT;
		u32 SampleCount = 1;
		u32 SampleQuality = 0;
	}*/
}

void RenderPassTest::Render(RenderDevice* device, RenderContext* ctx)
{

}

void RenderPassTest::AddPrimitive(PrimitiveSceneInfo* primitive)
{
	MeshCommand* command = new MeshCommand;
	command->VertexBuffers = primitive->VertexBuffers;
	command->IndexBuffer = primitive->IndexBuffer;
	command->PrimitiveId = primitive->PrimitiveId;

	
}

void RenderPassTest::RemovePrimitive(PrimitiveSceneInfo* primitive)
{

}