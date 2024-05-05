#pragma once

class DX12RHIModule final : public IRHIModule
{
public:
	virtual void Init() override;
	virtual void Destroy() override;
	virtual RenderDevice* CreateDevice() override;
	virtual RenderContext* CreateContext(u32 frameCount) override;

};
 