#include "Private.h"

IMPLEMENT_MODULE(RHIModule)
void RHIModule::Init(std::any hwnd,const RenderConfig& config)
{
	IRHIImplModule& RHIImpl = GetRHIImplModule(config.API);
	_Device = RHIImpl.CreateDevice();
	_Context = RHIImpl.CreateContext(config.FrameCount);
 
	constexpr u32 GlobalDescriptorNumber = 2000;
	constexpr u32 SamplerDescriptorNumber = 500;
	constexpr u32 RVTDescriptorNumber = 64;
	constexpr u32 DSVDescriptorNumber = 32;

	_GlobalDescriptorHeap = _Device->CreateDescriptorHeap(DescriptorType::CBV_SRV_UAV, GlobalDescriptorNumber, true);
	_SamplerDescriptorHeap = _Device->CreateDescriptorHeap(DescriptorType::Sampler, SamplerDescriptorNumber, true);
	_RVTDescriptorHeap = _Device->CreateDescriptorHeap(DescriptorType::RVT, RVTDescriptorNumber, false);
	_DSVDescriptorHeap = _Device->CreateDescriptorHeap(DescriptorType::DSV, DSVDescriptorNumber, false);

	SwapChain::Config sc = {
		.Width = config.FrameWidth,
		.Height = config.FrameHeight,
		.SampleCount = 1,
		.SampleQuality = 0,
		.BufferCount = config.FrameCount,
		.Format = PixelFormat::R8G8B8A8_UNORM,
	};

	_SwapChain = _Device->CreateSwapChain(sc, _Context->DirectCommandQueue(), _RVTDescriptorHeap, hwnd);
	_FrameIndex = _SwapChain->CurrentFrameIndex();

	
	_Fence = _Device->CreateFence(config.FrameCount);
	_Fence->Wait(_FrameIndex, _Context->DirectCommandQueue());

}

void RHIModule::PreRender()
{

}

void RHIModule::PostRender()
{

}

void RHIModule::Destroy()
{

}

 

