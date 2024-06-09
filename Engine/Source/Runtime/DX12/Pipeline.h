#pragma once

class DX12GraphicPipeline : public GraphicPipeline
{
	friend class DX12Device;
public:
	virtual ~DX12GraphicPipeline() { _Handle.Reset(); }
	virtual std::any Handle() override { return _Handle.Get(); }

private:
	DX12GraphicPipeline(ComPtr<ID3D12PipelineState> handle)
		:_Handle(handle)
	{}

	ComPtr<ID3D12PipelineState> _Handle;
};