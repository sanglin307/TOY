#pragma once

struct RenderCluster;
struct ClusterAllocInfo
{
	u64  PositionOffset;
	u64  VertexAttributeOffset;
	u32  IndexOffset;
};

class GVertexIndexBuffer
{
public:
	static GVertexIndexBuffer& Instance();
	void Init(RenderDevice* device);
	void Destroy();

	void Create(const std::vector<RenderCluster*>& clusters);

private:
	RenderDevice* _Device;
	u64  _AllocCount;
	RenderBuffer* _PositionBuffer = nullptr;
	RenderBuffer* _VertexAttributesBuffer = nullptr;
	RenderBuffer* _IndexBuffer = nullptr;
	std::vector<ClusterAllocInfo>  _ClusterAlloc;
};