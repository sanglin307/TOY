#pragma once

#include "RenderPath.h"

class VisibilityBufferRP : public RenderPath
{
public:
	virtual void Init() override;
	virtual void Render() override;
	virtual void Destroy() override;

private:
	CommandQueue* _DirectQueue = nullptr;
	CommandAllocator* _CommandAllocator = nullptr;
};