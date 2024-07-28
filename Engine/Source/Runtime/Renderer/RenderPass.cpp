#include "Private.h"

void RenderPassForward::Init(RenderDevice* device,SceneRenderer* renderer)
{
	_Type = RenderPassType::Forward;
	_Device = device;
	_Renderer = renderer;

	GraphicPipeline::Desc desc = {};
	ScenePso = static_cast<GraphicPipeline*>(device->CreateGraphicPipeline("ForwardPass",desc));

	Sampler::Desc sd;
	_Sampler = device->CreateSampler(sd);
	ScenePso->BindParameter("DefaultSampler", _Sampler);

	RenderBuffer::Desc bd = {
		.Size = sizeof(MaterialData),
		.Stride = sizeof(MaterialData),
		.Usage = (u32)ResourceUsage::UniformBuffer,
		.CpuAccess = CpuAccessFlags::Write,
		.Alignment = true
	};
	MaterialBuffer = device->CreateBuffer("MaterialBuffer", bd);
	ScenePso->BindParameter("MaterialCB", MaterialBuffer);

	ComputePipeline::Desc cd = {};
	TonemapPso = static_cast<ComputePipeline*>(device->CreateComputePipeline("TonemapCS", cd));
}

RenderPassForward::~RenderPassForward()
{
	for (auto iter : _Commands)
	{
		delete iter;
	}

	delete MaterialBuffer;
}

void RenderPassForward::Render(ViewInfo& view, Swapchain* sc, RenderContext* ctx)
{
	ctx->SetViewport(0, 0, view.ViewportSize.x, view.ViewportSize.y);
	ctx->SetScissorRect(0, 0, view.ViewportSize.x, view.ViewportSize.y);

	const SceneTextures& sceneTextures = _Renderer->GetSceneTextures();
	RenderScene* scene = _Renderer->GetScene();

	{
		RenderMarker marker(ctx, float3(1, 0, 0), "BasePass");
		RenderTexture* rts[] = { sceneTextures.SceneColor };
		ctx->SetRenderTargets(1, rts, RenderTargetColorFlags::Clear, sceneTextures.SceneDepth, RenderTargetDepthStencilFlags::Clear);

		RenderBuffer* drawDataBuffer = _Renderer->GetDrawDataBuffer();
		ctx->SetRootSignature(ScenePso->GetRootSignature(), PipelineType::Graphic);
		ScenePso->BindParameter("ViewCB", _Renderer->GetViewUniformBuffer());
		ScenePso->BindParameter("LightsBuffer", _Renderer->GetLightsBuffer());
		ScenePso->BindParameter("PrimitiveBuffer", _Renderer->GetPrimitivesBuffer());
		ScenePso->BindParameter("DrawCB", drawDataBuffer);
		ctx->SetPrimitiveTopology(ScenePso->Info.Topology);
		ctx->SetRenderPipeline(ScenePso);
 
		for (auto c : _Commands)
		{
			MaterialBuffer->UploadData((u8*)&(c->Material), sizeof(c->Material));
			if (c->Material.TextureMask & BaseColorTextureMask)
				ScenePso->BindParameter("BaseColorTexture", c->BaseColor);
			else
				ScenePso->BindParameter("BaseColorTexture", DefaultResource::Instance().GetColorBlackTexture());

			if (c->Material.TextureMask & NormalTextureMask)
				ScenePso->BindParameter("NormalTexture", c->Normal);
			else
				ScenePso->BindParameter("NormalTexture", DefaultResource::Instance().GetColorBlackTexture());

			if (c->Material.TextureMask & RoughnessMetalnessTextureMask)
				ScenePso->BindParameter("RoughnessMetalnessTexture", c->RoughMetelness);
			else
				ScenePso->BindParameter("RoughnessMetalnessTexture", DefaultResource::Instance().GetColorBlackTexture());

			if (c->Material.TextureMask & EmissiveTextureMask)
				ScenePso->BindParameter("EmissiveTexture", c->Emissive);
			else
				ScenePso->BindParameter("EmissiveTexture", DefaultResource::Instance().GetColorBlackTexture());

			ScenePso->CommitParameter(ctx);

			DrawData dd = {
				.PrimitiveId = c->PrimitiveID
			};
			drawDataBuffer->UploadData((u8*)&dd, sizeof(dd));
			
			ctx->SetVertexBuffers((u32)c->VertexBuffers.size(), c->VertexBuffers.data());

			ctx->SetRenderMarker(float3(0, 0, 1), "BeginDrawCommand");

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

	{
		RenderMarker marker(ctx, float3(0, 1, 0), "Tonemapping");
		ctx->SetRootSignature(TonemapPso->GetRootSignature(), PipelineType::Compute);
		ctx->SetRenderPipeline(TonemapPso);
		TonemapPso->BindParameter("ViewCB", _Renderer->GetViewUniformBuffer());
		TonemapPso->BindParameter("SceneColor", sceneTextures.SceneColor);
		TonemapPso->BindParameter("ColorUAV", sceneTextures.ColorOutput);
		TonemapPso->CommitParameter(ctx);
		ctx->Dispatch(DivideRoundup(view.ViewportSize.x, 16), DivideRoundup(view.ViewportSize.y, 16), 1);
	}

}

void RenderPassForward::AddCluster(u32 primitiveID, RenderCluster* cluster)
{
	RenderCommand* command = new RenderCommand;
	command->PrimitiveID = primitiveID;
	for (u32 i = 0; i < ScenePso->Info.VertexLayout.Desc.size(); i++)
	{
		InputLayoutDesc& desc = ScenePso->Info.VertexLayout.Desc[i];
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

void RenderPassForward::RemoveCluster(RenderCluster* cluster)
{
	auto iter = _Commands.begin();
	while (iter != _Commands.end())
	{
		if ((*iter)->Component == cluster->Component)
		{
			delete (*iter);
			iter = _Commands.erase(iter);
		}
		else
			iter++;
	}
}