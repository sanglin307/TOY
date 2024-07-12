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

class DX12RootSignature : public RootSignature
{
	friend class DX12Device;
public:
	virtual ~DX12RootSignature() { _Handle.Reset(); }
	virtual std::any Handle() const override { return _Handle.Get(); }
 
private:
	DX12RootSignature(const RootSignature::Desc& desc, const std::vector<RootSignatureParamDesc>& paramDesc,ComPtr<ID3D12RootSignature> handle)
	{
		_Info = desc;
		_ParamsDesc = paramDesc;
		_Handle = handle;
	}
	ComPtr<ID3D12RootSignature> _Handle;
};