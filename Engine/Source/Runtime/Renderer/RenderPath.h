#pragma once

class RenderDevice;
class SwapChain;

class RenderPath
{
public:
	virtual void Init();
	virtual void Render() = 0;
	virtual void Destroy();

protected:


};


