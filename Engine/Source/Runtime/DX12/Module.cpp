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


