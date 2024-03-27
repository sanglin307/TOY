#include "Private.h"

DX12GraphicPipeline::~DX12GraphicPipeline()
{
	if (_Desc.RootSignature != nullptr)
	{
		delete _Desc.RootSignature;  // todo. dedicated manager for root signature ?
		_Desc.RootSignature = nullptr;
	}

	_Handle.Reset(); 
}