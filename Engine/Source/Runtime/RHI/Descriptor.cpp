#include "Private.h"

DescriptorManager::DescriptorManager(RenderDevice* device)
{
    _Device = device;
    _ShaderResourceDescriptorHeap = _Device->CreateDescriptorHeap(DescriptorType::CBV_SRV_UAV, GlobalDescriptorNumber, true);
    _SamplerDescriptorHeap = _Device->CreateDescriptorHeap(DescriptorType::Sampler, SamplerDescriptorNumber, true);
    _RVTDescriptorHeap = _Device->CreateDescriptorHeap(DescriptorType::RVT, RVTDescriptorNumber, false);
    _DSVDescriptorHeap = _Device->CreateDescriptorHeap(DescriptorType::DSV, DSVDescriptorNumber, false);
}

DescriptorManager::~DescriptorManager()
{
    delete _ShaderResourceDescriptorHeap;
    _ShaderResourceDescriptorHeap = nullptr;

    delete _SamplerDescriptorHeap;
    _SamplerDescriptorHeap = nullptr;

    delete _RVTDescriptorHeap;
    _RVTDescriptorHeap = nullptr;

    delete _DSVDescriptorHeap;
    _DSVDescriptorHeap = nullptr;
}