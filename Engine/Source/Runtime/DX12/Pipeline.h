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
	virtual std::any Handle() override { return _Handle.Get(); }
 
private:
	DX12RootSignature(const RootSignature::Desc& desc, const std::vector<RootSignatureParamDesc>& paramDesc,ComPtr<ID3D12RootSignature> handle)
	{
		_Info = desc;
		_ParamsDesc = paramDesc;
		_Handle = handle;
	}
	ComPtr<ID3D12RootSignature> _Handle;
};


//class DX12GlobalRootSignature : public RootSignature
//{
//	friend class DX12Device;
//public:
//	DX12GlobalRootSignature(DX12Device* device);
//	virtual ~DX12GlobalRootSignature() { _Handle.Reset(); }
//	virtual std::any Handle() override { return _Handle.Get(); }
//
//	virtual ShaderParameter* Allocate(const SRBoundResource& res) override;
//	// limit.
//	constexpr static u32 cRootSignatureSizeLimit = 64;   // dword, from dx12 document.
//	constexpr static u32 cRootTableSize = 1; // dword.
//	constexpr static u32 cRootDescriptorsSize = 2; // dword.
//
//	constexpr static u32 cRootDescriptorsNumberMax = 12; 
//	constexpr static u32 cRootTableNumberMax = (cRootSignatureSizeLimit - cRootDescriptorsNumberMax * cRootDescriptorsSize) / cRootTableSize;
//	constexpr static u32 cRootTableOneParameterNumberMax = 32;  // table that contains 1 descriptor
//	constexpr static u32 cRootTableMultipleParameterNumberMax = cRootTableNumberMax - cRootTableOneParameterNumberMax; // table that contains multiple descriptors.
//
//	constexpr static u32 cRootTableDescriptorSizeMax = 100; // 1 table contain 100 descriptor.
//
//private:
//	ComPtr<ID3D12RootSignature> _Handle;
//
//	u32 RootParameterNumber[(u32)ShaderBindType::Max];
//
//	// offset from begin of root signature.
//	u32 RootParameterOffset[(u32)ShaderBindType::Max];
//
//	// used number
//	u32 RootParameterUsed[(u32)ShaderBindType::Max] = {};
//};