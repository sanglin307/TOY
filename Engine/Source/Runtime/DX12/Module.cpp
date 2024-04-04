#include "Private.h"

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

extern "C"  DX12_API ModuleInterface* CreateModule()
{
	return new DX12RHIModule;
}