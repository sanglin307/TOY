#include "Private.h"

RenderPassForward::RenderPassForward(RenderDevice* device,SceneRenderer* renderer)
	:RenderPass(RenderPassType::Forward,device,renderer)
{
	const SceneTextures& sceneTextures = _Renderer->GetSceneTextures();

	GraphicPipeline::Desc desc = {
		.VertexSlotMapping = InputSlotMapping::PositionSeperated,
		.VS = {
		   .Profile = ShaderProfile::Vertex,
		   .Path = "ForwardPass.hlsl",
		   .Entry = "VSMain"
        },
		.PS = {
			.Profile = ShaderProfile::Pixel,
			.Path = "ForwardPass.hlsl",
			.Entry = "PSMain"
	    },
		.RasterizerState = {
			.FrontCounterClockwise = true  // gltf model default winding.
        },
		.DepthStencilState = {
			.DepthEnable = true
        },
		.RVTFormats = { sceneTextures.SceneColor->GetFormat() },
		.DSVFormat = sceneTextures.SceneDepth->GetFormat()
	};
	_ScenePso = static_cast<GraphicPipeline*>(device->CreateGraphicPipeline("ForwardPass",desc));
}

RenderPassForward::~RenderPassForward()
{
	for (auto iter : _Commands)
	{
		delete iter;
	}
}

void RenderPassForward::Render(ViewInfo& view, Swapchain* sc, RenderContext* ctx)
{
	const SceneTextures& sceneTextures = _Renderer->GetSceneTextures();
	RenderScene* scene = _Renderer->GetScene();
	DynamicRingBuffer* ringBuffer = _Device->GetDynamicRingBuffer();
	{
		RenderMarker marker(ctx, float3(0.18, 0.18, 0.18), "ForwardPass");
		RenderTexture* rts[] = { sceneTextures.SceneColor };
		ctx->SetRenderTargets(1, rts, RenderTargetColorFlags::Clear, sceneTextures.SceneDepth, RenderTargetDepthStencilFlags::Clear);

		ctx->SetRootSignature(_ScenePso->GetRootSignature(), PipelineType::Graphic);
		 
		u32 voff = ringBuffer->AllocateConstBuffer((u8*)&view, sizeof(ViewInfo));
		_ScenePso->BindParameter("ViewCB", ringBuffer->GetResource(), voff);

		_ScenePso->BindParameter("LightsBuffer", _Renderer->GetLightsBuffer());
		_ScenePso->BindParameter("PrimitiveBuffer", _Renderer->GetPrimitivesBuffer());
		ctx->SetPrimitiveTopology(_ScenePso->Info.Topology);
		ctx->SetRenderPipeline(_ScenePso);
 
		const std::vector<ClusterAllocInfo>& allocInfo = GVertexIndexBuffer::Instance().GetClusterAllocInfo();
		for (u32 i=0;i < _Commands.size();i++)
		{
			RenderCommand* c = _Commands[i];
			ClusterAllocInfo alloc = allocInfo[i];

			DrawData dd = {
				.PrimitiveId = c->PrimitiveID,
			};

			u32 offset = ringBuffer->AllocateConstBuffer((u8*)&dd, sizeof(dd));
			_ScenePso->BindParameter("DrawCB", ringBuffer->GetResource(), offset);

			u32 matoff = ringBuffer->AllocateConstBuffer((u8*)&(c->Material), sizeof(c->Material));
			_ScenePso->BindParameter("MaterialCB", ringBuffer->GetResource(), matoff);

			if (c->Material.TextureMask & BaseColorTextureMask)
				_ScenePso->BindParameter("BaseColorTexture", c->BaseColor);
			else
				_ScenePso->BindParameter("BaseColorTexture", DefaultResource::Instance().GetColorBlackTexture());

			if (c->Material.TextureMask & NormalTextureMask)
				_ScenePso->BindParameter("NormalTexture", c->Normal);
			else
				_ScenePso->BindParameter("NormalTexture", DefaultResource::Instance().GetColorBlackTexture());

			if (c->Material.TextureMask & RoughnessMetalnessTextureMask)
				_ScenePso->BindParameter("RoughnessMetalnessTexture", c->RoughMetelness);
			else
				_ScenePso->BindParameter("RoughnessMetalnessTexture", DefaultResource::Instance().GetColorBlackTexture());

			if (c->Material.TextureMask & EmissiveTextureMask)
				_ScenePso->BindParameter("EmissiveTexture", c->Emissive);
			else
				_ScenePso->BindParameter("EmissiveTexture", DefaultResource::Instance().GetColorBlackTexture());
 
			_ScenePso->CommitParameter(ctx);

			RenderBuffer* vbs[] = { GVertexIndexBuffer::Instance().GetPositionBuffer(), GVertexIndexBuffer::Instance().GetVertexAttributeBuffer() };
			ctx->SetVertexBuffers(2, vbs);

			if (c->IndexBuffer)
			{
				ctx->SetIndexBuffer(GVertexIndexBuffer::Instance().GetIndexBuffer());
				u32 indexCount = u32(c->IndexBuffer->GetElementCount());
				ctx->DrawIndexedInstanced(alloc.IndexCount,1,alloc.IndexOffset,alloc.VertexOffset);
			}
			else
			{
				u32 vertexCount = u32(c->PositionBuffer->GetElementCount());
				ctx->DrawInstanced(vertexCount);
			}
			 
			
		}
	}
}

void RenderPassForward::AddCluster(u32 primitiveID, RenderCluster* cluster)
{
	RenderCommand* command = new RenderCommand;
	command->PrimitiveID = primitiveID;
	if (_ScenePso->Info.VertexSlotMapping == InputSlotMapping::Seperated)
	{
		for (u32 i = 0; i < _ScenePso->Info.VertexLayout.Desc.size(); i++)
		{
			InputLayoutDesc& desc = _ScenePso->Info.VertexLayout.Desc[i];
			VertexAttribute VA = _Device->TranslateSemanticToAttribute(desc.SemanticName, desc.SemanticIndex);
			if (VA == VertexAttribute::Position)
				continue;

			auto iter = std::find_if(cluster->VertexStreams.begin(), cluster->VertexStreams.end(), [&](RenderCluster::Stream& s) -> bool {

				if (s.Attribute == VA)
					return true;
				return false;
				});

			if (iter != cluster->VertexStreams.end())
			{
				command->VertexAttributeBuffers.push_back(iter->Buffer);
			}
			else
			{
				command->VertexAttributeBuffers.push_back(DefaultResource::Instance().GetVertexBuffer(VA));
			}
		}
	}
	command->PositionBuffer = cluster->PositionBuffer;
	command->CompactVertexAttributeBuffer = cluster->CompactVertexAttributeBuffer;
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

