#include "Private.h"

GameWorld& GameWorld::Instance()
{
	static GameWorld inst;
	return inst;
}


void GameWorld::Init()
{
	_RenderScene = GameEngine::Instance().GetRenderer().AllocateScene(this);

	u32 width, height;
	GameEngine::Instance().FrameSize(width, height);
	f32 aspectRatio = width * 1.f / height;

	float positionVertices[] =
	{
		 0.0f, 0.25f * aspectRatio, 0.0f,
		 0.25f, -0.25f * aspectRatio, 0.0f,
		 -0.25f, -0.25f * aspectRatio, 0.0f
	};

	float colorVertices[] =
	{
		 1.0f, 0.0f, 0.0f,
		 0.0f, 1.0f, 0.0f,
		 0.0f, 0.0f, 1.0f 
	};

	Mesh* mesh = new Mesh(PrimitiveTopology::Triangle);
	mesh->InsertAttribute(VertexAttribute::Position, VertexData{
		.Format = VertexFormat::Float32x3,
		.Data = (u8*)positionVertices,
		.Size = 3 * 3 * sizeof(float)
		});
	mesh->InsertAttribute(VertexAttribute::Color, VertexData{
		.Format = VertexFormat::Float32x3,
		.Data = (u8*)colorVertices,
		.Size = 3 * 3 * sizeof(float)
		});
	
	PrimitiveComponent* pc = new PrimitiveComponent;
	pc->MeshData = mesh;
	_Primitives.push_back(pc);
	_RenderScene->AddPrimitive(pc);
}

void GameWorld::Destroy()
{
	for (PrimitiveComponent* p : _Primitives)
	{
		delete p;
	}
	_Primitives.clear();
	
	GameEngine::Instance().GetRenderer().RemoveScene(_RenderScene);
}

void GameWorld::Update(double delta)
{

}