#include "Private.h"

IMPLEMENT_MODULE(DX12RHIModule)

static DX12Device* sDevice = nullptr;
void DX12RHIModule::Init()
{
    check(!sDevice);
    sDevice = new DX12Device;
}

void DX12RHIModule::Destroy()
{
    delete sDevice;
    sDevice = nullptr;
}

RenderDevice* DX12RHIModule::GetDevice()
{
    return sDevice;
}

 