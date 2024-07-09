#include "Private.h"

void RenderPass::BindVertexStreams(GraphicPipeline* pso, RenderCluster* cluster, RenderContext* ctx)
{
	std::vector<RenderBuffer*> vbs;
	std::vector<u64> vbOffset;
	for (u32 i = 0; i < pso->Info.VertexLayout.Desc.size(); i++)
	{
		InputLayoutDesc& desc = pso->Info.VertexLayout.Desc[i];
		VertexAttribute VA = _Device->TranslateSemanticToAttribute(desc.SemanticName, desc.SemanticIndex);
		auto iter = std::find_if(cluster->VertexStreams.begin(), cluster->VertexStreams.end(), [&](RenderCluster::Stream& s) -> bool {
			
			if (s.Attribute == VA)
				return true;
			return false;
		});
		
		if (iter != cluster->VertexStreams.end())
		{
			check(desc.SlotOffset == iter->ByteOffset);
			vbs.push_back(iter->Buffer);
			vbOffset.push_back(iter->ByteOffset);
		}
		else
		{
			vbs.push_back(DefaultResource::Instance().GetVertexBuffer(VA));
			vbOffset.push_back(iter->ByteOffset);
		}
	}

	ctx->SetVertexBuffers((u32)vbs.size(), vbs.data(), vbOffset.data());
	
	if (cluster->IndexBuffer)
	{
		ctx->SetIndexBuffer(cluster->IndexBuffer);
	}
}

void RenderPassTest::Init(RenderDevice* device,SceneRenderer* renderer)
{
	_Type = RenderPassType::Test;
	_Device = device;
	_Renderer = renderer;

	GraphicPipeline::Desc desc = {
		.Name = "TestPSO"
	};
	PSO = device->CreateGraphicPipeline(desc);
	PSO->BindParameter("ViewCB", renderer->GetViewUniformBuffer());
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
	for (auto c : _Clusters)
	{
		std::vector<RenderBuffer*> buffers;
		BindVertexStreams(PSO, c, ctx);

		if (c->IndexBuffer)
		{
			u32 indexCount = u32(c->IndexBuffer->GetSize() / c->IndexBuffer->GetStride());
			ctx->DrawIndexedInstanced(indexCount);
		}
		else
		{
			u32 vertexCount = u32(c->VertexStreams[0].Buffer->GetSize() / c->VertexStreams[0].Buffer->GetStride());
			ctx->DrawInstanced(vertexCount);
		}
	}
}

void RenderPassTest::AddCluster(RenderCluster* cluster)
{
	_Clusters.insert(cluster);
}

void RenderPassTest::RemoveCluster(RenderCluster* cluster)
{
	_Clusters.erase(cluster);
}