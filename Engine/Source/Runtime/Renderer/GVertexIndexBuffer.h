#pragma once

struct RenderCluster;
struct ClusterAllocInfo
{
	u32  VertexOffset;
	u32  VertexCount;
	u32  IndexOffset;
	u32  IndexCount;
};

class GVertexIndexBuffer
{
public:
	static GVertexIndexBuffer& Instance();
	void Init(RenderDevice* device);
	void Destroy();

	void Create(const std::vector<RenderCluster*>& clusters);

	const std::vector<ClusterAllocInfo>& GetClusterAllocInfo() const
	{
		return _ClusterAlloc;
	}

	RenderBuffer* GetPositionBuffer()
	{
		return _PositionBuffer;
	}

	RenderBuffer* GetVertexAttributeBuffer()
	{
		return _VertexAttributesBuffer;
	}

	RenderBuffer* GetIndexBuffer()
	{
		return _IndexBuffer;
	}

private:
	RenderDevice* _Device;
	RenderBuffer* _PositionBuffer = nullptr;
	RenderBuffer* _VertexAttributesBuffer = nullptr;
	RenderBuffer* _IndexBuffer = nullptr;
	std::vector<ClusterAllocInfo>  _ClusterAlloc;
};