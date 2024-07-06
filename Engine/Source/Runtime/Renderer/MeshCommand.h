#pragma once

struct MeshVertexBuffer
{
	VertexAttribute Attribute;
	RenderBuffer*   Buffer;
};


struct MeshCommand
{
	std::vector<MeshVertexBuffer>  VertexBuffers;
	RenderBuffer* IndexBuffer = nullptr;

	PrimitiveComponent* Component = nullptr;
	GraphicPipeline* PSO = nullptr;
};