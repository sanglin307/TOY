#include "Private.h"

IMPLEMENT_MODULE(DX12RHIModule)

void DX12RHIModule::Init()
{

}

void DX12RHIModule::Destroy()
{

}

RenderDevice* DX12RHIModule::CreateDevice()
{
	return new DX12Device;
}

RenderContext* DX12RHIModule::CreateContext(u32 frameCount)
{
	return new DX12RenderContext(frameCount);
}


