#pragma once

class VisibilityBufferRP : public RenderPath
{
public:
	virtual void Init() override;
	virtual void Render() override;
	virtual void Destroy() override;

private:
	
};