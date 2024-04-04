#pragma once

class DX12RHIModule : public RHIModule
{
public:
	virtual void Init() override;
	virtual void Destroy() override;
	virtual RenderDevice* CreateDevice() override;

};
 