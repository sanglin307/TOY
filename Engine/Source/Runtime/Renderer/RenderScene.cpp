#include "Private.h"

RenderCluster::~RenderCluster()
{
	for (auto v : VertexBuffers)
	{
		delete v.Buffer;
	}

	if (IndexBuffer)
		delete IndexBuffer;
}

RenderScene::RenderScene(GameWorld* world,SceneRenderer* renderer)
{
	_World = world;
	_Renderer = renderer;
	renderer->BindScene(this);
}

RenderScene::~RenderScene()
{
	for (RenderCluster* p : _Clusters)
	{
		delete p;
	}
}

void RenderScene::AddPrimitive(PrimitiveComponent* primitive)
{
	auto iter = std::find_if(_Clusters.begin(), _Clusters.end(), [primitive](const RenderCluster* info) -> bool {
		return primitive == info->Component;
		});

	check(iter == _Clusters.end());

	RenderDevice* device = _Renderer->GetDevice();
	if (primitive->GetType() == PrimitiveType::Mesh)
	{
		RenderCluster* cluster = new RenderCluster;
		cluster->Component = primitive;

		Mesh* mesh = static_cast<Mesh*>(primitive);
		const std::vector<MeshSegment*>& segments = mesh->GetSegments();
		for (MeshSegment* s : segments)
		{
			const std::array<VertexData, (u32)VertexAttribute::Max>& vertexData = s->GetVertexData();
			for (u32 i = 0; i < vertexData.size(); i++)
			{
				const VertexData& d = vertexData[i];
				if (!d.Size)
					continue;

				RenderBuffer::Desc desc = {
					 .Size = d.Size,
					 .Stride = d.GetStride(),
					 .Usage = (u32)ResourceUsage::VertexBuffer,
					 .CpuAccess = 0,
					 .Alignment = true,
					 .InitData = d.Data
				};

				cluster->VertexBuffers.push_back(MeshVertexBuffer{
					.Attribute = (VertexAttribute)i,
					.Buffer = device->CreateBuffer(std::format("VertexBuffer_{}_{}",(u64)(primitive),i),desc)
					});
			}

			const VertexData& indexData = s->GetIndexData();
			if (indexData.Size > 0)
			{
				RenderBuffer::Desc desc = {
					 .Size = indexData.Size,
					 .Stride = indexData.GetStride(),
					 .Usage = (u32)ResourceUsage::IndexBuffer,
					 .CpuAccess = 0,
					 .Alignment = true,
					 .InitData = indexData.Data
				};

				cluster->IndexBuffer = device->CreateBuffer(std::format("IndexBuffer_{}", (u64)(primitive)), desc);
			}

			_Clusters.push_back(cluster);
			_Renderer->AddCluster(cluster);
		}
	}
	else
	{
		check(0);
	}
	
}

void RenderScene::RemovePrimitive(PrimitiveComponent* primitive)
{
	auto iter = _Clusters.begin();
	while (iter != _Clusters.end())
	{
		if ((*iter)->Component == primitive)
		{
			_Renderer->RemoveCluster(*iter);
			delete *iter;
			auto temp = iter++;
			_Clusters.erase(temp);
		}
		else
		{
			iter++;
		}
	}
}

