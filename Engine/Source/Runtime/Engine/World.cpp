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

	hlslpp::float3 positionVertices[] =
	{
		{ 0.0f, 0.25f * aspectRatio, 0.0f },
		{ 0.25f, -0.25f * aspectRatio, 0.0f },
		{ -0.25f, -0.25f * aspectRatio, 0.0f }
	};

	hlslpp::float3 colorVertices[] =
	{
		{ 1.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f } 
	};

	Mesh* mesh = Mesh::Create(PrimitiveTopology::Triangle);
	mesh->InsertAttribute(VertexAttribute::Position, VertexData{
		.Format = VertexFormat::Float32x3,
		.Data = (u8*)positionVertices,
		.Size = 3 * sizeof(hlslpp::float3)
		});
	mesh->InsertAttribute(VertexAttribute::Color, VertexData{
		.Format = VertexFormat::Float32x3,
		.Data = (u8*)colorVertices,
		.Size = 3 * sizeof(hlslpp::float3)
		});
	
	_Meshes.push_back(mesh);
}

void GameWorld::Destroy()
{
	for (Mesh* mesh : _Meshes)
	{
		delete mesh;
	}
	_Meshes.clear();
	
	GameEngine::Instance().GetRenderer().RemoveScene(_RenderScene);
}

void GameWorld::Update(double delta)
{

}