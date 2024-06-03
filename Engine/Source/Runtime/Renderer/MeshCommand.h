#pragma once

struct MeshVertexBuffer
{
	VertexAttribute Attribute;
	RenderBuffer*   Buffer;
};

struct MeshShaderBind
{
	ShaderResource* Shader;
	std::map<std::string,RenderResource*> Parameters;
};

struct MeshCommand
{
	std::vector<MeshVertexBuffer>  VertexBuffers;
	RenderBuffer* IndexBuffer = nullptr;

	std::array<MeshShaderBind,(u32)ShaderProfile::MAX> ShaderBinds;
	GraphicPipeline* PSO = nullptr;
};