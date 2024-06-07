#include "Private.h"

RenderScene::RenderScene(GameWorld* world,SceneRenderer* renderer)
{
	_World = world;
	_Renderer = renderer;
	renderer->BindScene(this);
}

RenderScene::~RenderScene()
{
}

void RenderScene::AddPrimitive(PrimitiveComponent* primitive)
{
	auto iter = std::find_if(_Primitives.begin(), _Primitives.end(), [primitive](const PrimitiveSceneInfo* info) -> bool {
		return primitive->PrimitiveId == info->PrimitiveId;
		});

	check(iter == _Primitives.end());

	PrimitiveSceneInfo* info = new PrimitiveSceneInfo;
	info->PrimitiveId = primitive->PrimitiveId;

	const std::array<VertexData,(u32)VertexAttribute::Max>& vertexData = primitive->MeshData->GetVertexData();

	RenderDevice* device = _Renderer->GetDevice();
	for (u32 i=0;i < vertexData.size(); i++)
	{
		const VertexData& d = vertexData[i];
		RenderBuffer::Desc desc = {
			 .Size = d.Size,
			 .Stride = d.GetStride(),
			 .Name = std::format("VertexBuffer_{}_{}",primitive->PrimitiveId,i),
			 .Usage = (u32)ResourceUsage::VertexBuffer,
			 .Alignment = true,
			 .InitData = d.Data
		};

		info->VertexBuffers.push_back(MeshVertexBuffer{
			.Attribute = (VertexAttribute)i,
			.Buffer = device->CreateBuffer(desc)
			});
	}

	_Primitives.push_back(info);
	_Renderer->AddPrimitive(info);
}

void RenderScene::RemovePrimitive(PrimitiveComponent* primitive)
{
	auto iter = std::find_if(_Primitives.begin(), _Primitives.end(), [primitive](const PrimitiveSceneInfo* info) -> bool {
		           return primitive->PrimitiveId == info->PrimitiveId;
		        });
	check(iter != _Primitives.end());

	_Renderer->RemovePrimitive(*iter);

	delete *iter;
	_Primitives.erase(iter);
}

