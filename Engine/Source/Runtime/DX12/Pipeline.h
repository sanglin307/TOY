#pragma once

class DX12RootSignature : public RootSignature
{
	friend class DX12Device;
public:
	virtual ~DX12RootSignature() { _Handle.Reset(); }
	virtual std::any Handle() { return _Handle.Get(); }
private:
	DX12RootSignature(ComPtr<ID3D12RootSignature> handle)
		:_Handle(handle)
	{}

	ComPtr<ID3D12RootSignature> _Handle;
};


class DX12GraphicPipeline : public GraphicPipeline
{
	friend class DX12Device;
public:
	virtual ~DX12GraphicPipeline() { _Handle.Reset(); }

private:
	DX12GraphicPipeline(ComPtr<ID3D12PipelineState> handle)
		:_Handle(handle)
	{}

	ComPtr<ID3D12PipelineState> _Handle;
};