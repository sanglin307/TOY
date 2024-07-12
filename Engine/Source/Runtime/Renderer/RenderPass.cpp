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

	Sampler::Desc sd;
	_Sampler = device->CreateSampler(sd);
	PSO->BindParameter("TestSampler", _Sampler);

	RenderBuffer::Desc bd = {
		.Size = sizeof(MaterialData),
		.Stride = sizeof(MaterialData),
		.Usage = (u32)ResourceUsage::UniformBuffer,
		.CpuAccess = CpuAccessFlags::Write,
		.Alignment = true
	};
	MaterialBuffer = device->CreateBuffer("MaterialBuffer", bd);
	PSO->BindParameter("MaterialCB", MaterialBuffer);
}

RenderPassTest::~RenderPassTest()
{
	for (auto iter : _Commands)
	{
		delete iter;
	}

	delete MaterialBuffer;
}

void RenderPassTest::Render(RenderDevice* device, RenderContext* ctx)
{
	ctx->SetRootSignature(PSO->GetRootSignature());
	PSO->BindParameter("ViewCB", _Renderer->GetViewUniformBuffer());
	ctx->SetPrimitiveTopology(PSO->Info.Topology);
	ctx->SetGraphicPipeline(PSO);

	for (auto c : _Commands)
	{
		MaterialBuffer->UploadData((u8*)&(c->Material), sizeof(c->Material));
		if (c->Material.TextureMask & BaseColorTextureMask)
			PSO->BindParameter("BaseColorTexture", c->BaseColor);

		if (c->Material.TextureMask & NormalTextureMask)
			PSO->BindParameter("NormalTexture", c->Normal);

		if (c->Material.TextureMask & RoughnessMetalnessTextureMask)
			PSO->BindParameter("RoughnessMetalnessTexture", c->RoughMetelness);

		if (c->Material.TextureMask & EmissiveTextureMask)
			PSO->BindParameter("EmissiveTexture", c->Emissive);

		PSO->CommitParameter(ctx);

		ctx->SetVertexBuffers((u32)c->VertexBuffers.size(), c->VertexBuffers.data());
		if (c->IndexBuffer)
		{
			ctx->SetIndexBuffer(c->IndexBuffer);
			u32 indexCount = u32(c->IndexBuffer->GetSize() / c->IndexBuffer->GetStride());
			ctx->DrawIndexedInstanced(indexCount);
		}
		else
		{
			u32 vertexCount = u32(c->VertexBuffers[0]->GetSize() / c->VertexBuffers[0]->GetStride());
			ctx->DrawInstanced(vertexCount);
		}
	}
}

void RenderPassTest::AddCluster(RenderCluster* cluster)
{
	RenderCommand* command = new RenderCommand;
	for (u32 i = 0; i < PSO->Info.VertexLayout.Desc.size(); i++)
	{
		InputLayoutDesc& desc = PSO->Info.VertexLayout.Desc[i];
		VertexAttribute VA = _Device->TranslateSemanticToAttribute(desc.SemanticName, desc.SemanticIndex);
		auto iter = std::find_if(cluster->VertexStreams.begin(), cluster->VertexStreams.end(), [&](RenderCluster::Stream& s) -> bool {

			if (s.Attribute == VA)
				return true;
			return false;
			});

		if (iter != cluster->VertexStreams.end())
		{
			check(desc.SlotOffset == iter->ByteOffset);
			command->VertexBuffers.push_back(iter->Buffer);
		}
		else
		{
			command->VertexBuffers.push_back(DefaultResource::Instance().GetVertexBuffer(VA));
		}
	}
	command->IndexBuffer = cluster->IndexBuffer;
	command->Component = cluster->Component;
	command->Material.BaseColorFactor = cluster->Material->BaseColorFactor;
	command->Material.EmissiveFactor = float4(cluster->Material->EmissiveFactor,1);
	command->Material.MetalnessFactor = cluster->Material->MetalnessFactor;
	command->Material.RoughnessFactor = cluster->Material->RoughnessFactor;
	command->Material.AlphaCutoff = cluster->Material->AlphaCutoff;

	command->Material.TextureMask = 0;
	if (cluster->Material->BaseColorTexture.Texture)
	{
		command->Material.TextureMask |= BaseColorTextureMask;
		command->BaseColor = cluster->Material->BaseColorTexture.Texture;
	}

	if (cluster->Material->EmissiveTexture.Texture)
	{
		command->Material.TextureMask |= EmissiveTextureMask;
		command->Emissive = cluster->Material->EmissiveTexture.Texture;
	}

	if (cluster->Material->RoughnessMetalnessTexture.Texture)
	{
		command->Material.TextureMask |= RoughnessMetalnessTextureMask;
		command->RoughMetelness = cluster->Material->RoughnessMetalnessTexture.Texture;
	}

	if (cluster->Material->NormalTexture.Texture)
	{
		command->Material.TextureMask |= NormalTextureMask;
		command->Normal = cluster->Material->NormalTexture.Texture;
	}

	_Commands.push_back(command);

}

void RenderPassTest::RemoveCluster(RenderCluster* cluster)
{
	auto iter = _Commands.begin();
	while (iter != _Commands.end())
	{
		if ((*iter)->Component == cluster->Component)
		{
			auto diter = iter++;
			delete (*diter);
			_Commands.erase(diter);
		}
		else
			iter++;
	}
}