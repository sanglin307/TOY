#include "Private.h"

void RenderPassTest::Init(RenderDevice* device,SceneRenderer* renderer)
{
	_Type = RenderPassType::Test;
	_Device = device;
	_Renderer = renderer;

	GraphicPipeline::Desc desc = {
		.Name = "TestPSO"
	};
	PSO = device->CreateGraphicPipeline(desc);
	PSO->BindParameter("ViewInfo", renderer->GetViewUniformBuffer());
	/*RenderBuffer::Desc udesc = {
		.Size = sizeof(SceneConstantBuffer),
		.Usage = (u32)ResourceUsage::UniformBuffer,
		.CpuAccess = (u32)CpuAccessFlags::Write,
		.Alignment = true
	};
	UniformBuffer = device->CreateBuffer("SceneConstantBuffer",udesc);
	PSO->BindParameter("SceneConstantBuffer", UniformBuffer);

	Image* image1 = ImageReader::Instance().LoadFromFile("GTA.png");
	RenderTexture::Desc td1 = {
		.Width = image1->Width,
		.Height = image1->Height,
		.DepthOrArraySize = 1,
		.MipLevels = 1,
		.Format = image1->Format,
		.Usage = (u32)ResourceUsage::ShaderResource,
		.Dimension = ResourceDimension::Texture2D,
		.Data = image1->Data,
		.Size = image1->Size
	};
	_texture1 = device->CreateTexture("GTA.png",td1);
	PSO->BindParameter("texture1", _texture1);

	Image* image2 = ImageReader::Instance().LoadFromFile("Valorant.png");
	RenderTexture::Desc td2 = {
		.Width = image2->Width,
		.Height = image2->Height,
		.DepthOrArraySize = 1,
		.MipLevels = 1,
		.Format = image2->Format,
		.Usage = (u32)ResourceUsage::ShaderResource,
		.Dimension = ResourceDimension::Texture2D,
		.Data = image2->Data,
		.Size = image2->Size
	};
	_texture2 = device->CreateTexture("Valorant", td2);
	PSO->BindParameter("texture2", _texture2);

	

	Sampler::Desc sd;
	_sampler = device->CreateSampler(sd);
	PSO->BindParameter("g_sampler", _sampler);*/
}

RenderPassTest::~RenderPassTest()
{
	/*delete UniformBuffer;
	delete _texture1;
	delete _texture2;
	delete _sampler;*/
}

void RenderPassTest::Render(RenderDevice* device, RenderContext* ctx)
{
	//update 
	/*const float translationSpeed = 0.005f;
	const float offsetBounds = 1.25f;

	UniformData.offset[0] += translationSpeed;
	if (UniformData.offset[0] > offsetBounds)
	{
		UniformData.offset[0] = -offsetBounds;
	}
	UniformBuffer->UploadData((u8*)&UniformData, sizeof(UniformData));*/

	ctx->SetGraphicPipeline(PSO);
	for (auto c : _Commands)
	{
		std::vector<RenderBuffer*> buffers;
		for (auto v : c->VertexBuffers)
		{
			buffers.push_back(v.Buffer);
		}

		if (c->IndexBuffer)
		{
			ctx->DrawIndexedInstanced((u32)buffers.size(), buffers.data(), c->IndexBuffer);
		}
		else
		{
			ctx->DrawInstanced((u32)buffers.size(), buffers.data());
		}
	}
}

void RenderPassTest::AddCluster(RenderCluster* cluster)
{
	MeshCommand* command = new MeshCommand;
	command->VertexBuffers = cluster->VertexBuffers;
	command->IndexBuffer = cluster->IndexBuffer;
	command->Component = cluster->Component;
	command->PSO = PSO;
	_Commands.push_back(command);
}

void RenderPassTest::RemoveCluster(RenderCluster* cluster)
{

}