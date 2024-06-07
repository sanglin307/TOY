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

	u64 PrimitiveId;
	GraphicPipeline* PSO = nullptr;
};