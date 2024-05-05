#include "Private.h"

GameWorld& GameWorld::Instance()
{
	static GameWorld inst;
	return inst;
}


void GameWorld::Init()
{
	std::array<ShaderObject*, (size_t)ShaderProfile::MAX> shaders;
	ShaderObject* vs = ShaderCompiler::CompileHLSL(ShaderCompiler::Args{
		.Profile = ShaderProfile::Vertex,
		.FileName = "shader.hlsl",
		.EntryName = "VSMain",
		.Debug = true
		});

	ShaderObject* ps = ShaderCompiler::CompileHLSL(ShaderCompiler::Args{
		.Profile = ShaderProfile::Pixel,
		.FileName = "shader.hlsl",
		.EntryName = "PSMain",
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

	PostionVertexData* pos = PostionVertexData::Create(positionVertices, 3);
	ColorVertexData* color = ColorVertexData::Create(colorVertices, 3);
	VertexFormat* format = new StaticMeshVF;
	StaticMesh* mesh = StaticMesh::Create(format, pos, color, mat);
	
	_Meshes.push_back(mesh);
}

void GameWorld::Destroy()
{
	for (StaticMesh* mesh : _Meshes)
	{
		delete mesh;
	}
	_Meshes.clear();
}

void GameWorld::Update(double delta)
{

}