#pragma once

enum class RenderPassType
{
	Test = 0,
	MAX
};


class RenderPass
{
public:
	virtual void Init(RenderDevice* device) = 0;
	virtual void Render(RenderDevice* device, RenderContext* ctx) = 0;
	virtual void AddCommand(MeshCommand* command) = 0;

protected:
	RenderPassType _Type;
};


class RenderPassTest final : public RenderPass
{
public:
	virtual void Init(RenderDevice* device) override;
	virtual void Render(RenderDevice* device, RenderContext* ctx) override;
	virtual void AddCommand(MeshCommand* command) override;
private:
	std::vector<MeshCommand*> _Commands;
};