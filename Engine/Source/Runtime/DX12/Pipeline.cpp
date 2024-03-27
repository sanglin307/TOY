#include "Private.h"

DX12GraphicPipeline::~DX12GraphicPipeline()
{
	_Handle.Reset();
}