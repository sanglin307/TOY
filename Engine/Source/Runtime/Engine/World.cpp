#include "Private.h"

GameWorld& GameWorld::Instance()
{
	static GameWorld inst;
	return inst;
}


void GameWorld::Init()
{
	_RenderScene = GameEngine::Instance().GetRenderer().AllocateScene(this);

	std::array<ShaderResource*, (size_t)ShaderProfile::MAX> shaders;
	ShaderResource* vs = ShaderCompiler::CompileHLSL(ShaderCompiler::Args{
		.Profile = ShaderProfile::Vertex,
		.Path = "shader.hlsl",
		.EntryPoint = "VSMain",
		.Debug = true
		});

	ShaderResource* ps = ShaderCompiler::CompileHLSL(ShaderCompiler::Args{
		.Profile = ShaderProfile::Pixel,
		.Path = "shader.hlsl",
		.EntryPoint = "PSMain",
		.Debug = true
		});

	if (vs)
	{
		shaders[(i32)ShaderProfile::Vertex] = vs;
	}

	if (ps)
	{
		shaders[(i32)ShaderProfile::Pixel] = ps;
	}

	Material* mat = Material::Create(shaders);

	u32 width, height;
	GameEngine::Instance().FrameSize(width, height);
	f32 aspectRatio = width * 1.f / height;

	Vector3f positionVertices[] =
	{
		{ 0.0f, 0.25f * aspectRatio, 0.0f },
		{ 0.25f, -0.25f * aspectRatio, 0.0f },
		{ -0.25f, -0.25f * aspectRatio, 0.0f }
	};

	Vector3f colorVertices[] =
	{
		{ 1.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f } 
	};

	Mesh* mesh = Mesh::Create(PrimitiveTopology::Triangle);
	mesh->InsertAttribute(VertexAttribute::Position, VertexData{
		.Format = VertexFormat::Float32x3,
		.Data = (u8*)positionVertices,
		.Size = 3 * sizeof(Vector3f)
		});
	mesh->InsertAttribute(VertexAttribute::Color, VertexData{
		.Format = VertexFormat::Float32x3,
		.Data = (u8*)colorVertices,
		.Size = 3 * sizeof(Vector3f)
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