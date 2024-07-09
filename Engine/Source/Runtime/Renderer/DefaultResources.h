#pragma once

class DefaultResource
{
public:
	static DefaultResource& Instance();

	RenderBuffer* GetVertexBuffer(VertexAttribute attribute)
	{
		return _VertexBuffers[(u32)attribute];
	}
	void Init(RenderDevice* device);
	void Destroy();
private:
	DefaultResource() = default;
	DefaultResource(const DefaultResource& rhs) = delete;
	DefaultResource(DefaultResource&& rhs) = delete;
	DefaultResource& operator=(const DefaultResource& rhs) = delete;
	DefaultResource& operator=(DefaultResource&& rhs) = delete;

private:
	std::array<RenderBuffer*, (u32)VertexAttribute::Max> _VertexBuffers = {};
	std::set<RenderResource*> _Resources;
};