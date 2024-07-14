#pragma once

enum class InputSlotClass
{
	PerVertex = 0,
	PerInstance
};

enum class InputSlotMapping
{
	Interleaved = 0,
	Seperated,
	Custom
};

struct InputLayoutDesc
{
	std::string SemanticName;
	u32 SemanticIndex;
	PixelFormat Format;
	u32 SlotIndex;
	u32 SlotOffset;
	InputSlotClass SlotClass;
	u32 InstanceStepRate = 0;
};

struct InputLayout
{
	std::vector<InputLayoutDesc> Desc;

	void HashUpdate(std::any handle) const
	{
		for (u32 i = 0; i < Desc.size(); i++)
		{
			const InputLayoutDesc& desc = Desc[i];
			HashStreamUpdate(handle, desc.SemanticName.c_str(), desc.SemanticName.length());
			HashStreamUpdate(handle, &desc.SemanticIndex, sizeof(desc.SemanticIndex));
			HashStreamUpdate(handle, &desc.Format, sizeof(desc.Format));
			HashStreamUpdate(handle, &desc.SlotIndex, sizeof(desc.SlotIndex));
			HashStreamUpdate(handle, &desc.SlotOffset, sizeof(desc.SlotOffset));
			HashStreamUpdate(handle, &desc.SlotClass, sizeof(desc.SlotClass));
			HashStreamUpdate(handle, &desc.InstanceStepRate, sizeof(desc.InstanceStepRate));
		}
	}
};


 
enum class FillMode
{
	WireFrame,
	Solid,
};

enum class CullMode
{
	None = 0,
	Front,
	Back
};

enum class LogicOp
{
	Clear = 0,
	Set,
	Copy,
    CopyInverted,
	Noop,
	Invert,
	And,
	Nand,
	Or,
	Nor,
	Xor,
	Equiv,
	AndReverse,
	AndInverted,
	OrReverse,
	OrInverted
};

enum class BlendFactor
{
	Zero = 0,
	One,
	SrcColor,
	InvSrcColor,
	SrcAlpha,
	InvSrcAlpha,
	DestAlpha,
	InvDestAlpha,
	DestColor,
	InvDestColor,
	SrcAlphaSat,
	BlendFactor,
	InvBlendFactor,
	Src1Color,
	InvSrc1Color,
	Src1Alpha,
	InvSrc1Alpha,
	AlphaFactor,
	InvAlphaFactor,
};

enum class BlendOp
{
	Add,
	Subtract,
	RevSubtract,
	Min,
	MAX
};

enum class PrimitiveTopology 
{
	Point,
	Line,
	Triangle
};


struct RasterizerDesc
{
	FillMode Fill = FillMode::Solid;
	CullMode Cull = CullMode::Back;
	bool FrontCounterClockwise = false;
	bool DepthClip = true;
	bool Multisample = false;
	bool LineAA = false;
	bool ConservativeMode = false;
	i32 DepthBias = 0;
	float DepthBiasClamp =0.f;
	float SlopScaledDepthBias = 0.f;
	u32 SampleCount = 0;

	void HashUpdate(std::any handle) const
	{
		HashStreamUpdate(handle, &Fill, sizeof(Fill));
		HashStreamUpdate(handle, &Cull, sizeof(Cull));
		HashStreamUpdate(handle, &FrontCounterClockwise, sizeof(FrontCounterClockwise));
		HashStreamUpdate(handle, &DepthClip, sizeof(DepthClip));
		HashStreamUpdate(handle, &Multisample, sizeof(Multisample));
		HashStreamUpdate(handle, &LineAA, sizeof(LineAA));
		HashStreamUpdate(handle, &ConservativeMode, sizeof(ConservativeMode));
		HashStreamUpdate(handle, &DepthBias, sizeof(DepthBias));
		HashStreamUpdate(handle, &DepthBiasClamp, sizeof(DepthBiasClamp));
		HashStreamUpdate(handle, &SlopScaledDepthBias, sizeof(SlopScaledDepthBias));
		HashStreamUpdate(handle, &SampleCount, sizeof(SampleCount));
	}
};



constexpr static u32 MaxRenderTargetNumber = 8;

struct BlendDesc
{
	struct RenderTarget
	{
		bool Enable = false;
		bool LogicOpEnable = false;
		BlendFactor Src = BlendFactor::One;
		BlendFactor Dest = BlendFactor::Zero;
		BlendOp Op = BlendOp::Add;
		BlendFactor SrcAlpha = BlendFactor::One;
		BlendFactor DestAlpha = BlendFactor::Zero;
		BlendOp AlphaOp = BlendOp::Add;
		LogicOp LogicOp = LogicOp::Noop;
		u8 WriteMask = 0x0f;

	};
	bool AlphaToCoverageEnable = false;
	bool IndependentBlendEnable = false;
	RenderTarget RenderTargets[MaxRenderTargetNumber];

	void HashUpdate(std::any handle) const
	{
		HashStreamUpdate(handle, &AlphaToCoverageEnable, sizeof(AlphaToCoverageEnable));
		HashStreamUpdate(handle, &IndependentBlendEnable, sizeof(IndependentBlendEnable));
		for (u32 i = 0; i < MaxRenderTargetNumber; i++)
		{
			const BlendDesc::RenderTarget& rt = RenderTargets[i];
			HashStreamUpdate(handle, &rt.Enable, sizeof(rt.Enable));
			HashStreamUpdate(handle, &rt.LogicOpEnable, sizeof(rt.LogicOpEnable));
			HashStreamUpdate(handle, &rt.Src, sizeof(rt.Src));
			HashStreamUpdate(handle, &rt.Dest, sizeof(rt.Dest));
			HashStreamUpdate(handle, &rt.Op, sizeof(rt.Op));
			HashStreamUpdate(handle, &rt.SrcAlpha, sizeof(rt.SrcAlpha));
			HashStreamUpdate(handle, &rt.DestAlpha, sizeof(rt.DestAlpha));
			HashStreamUpdate(handle, &rt.AlphaOp, sizeof(rt.AlphaOp));
			HashStreamUpdate(handle, &rt.LogicOp, sizeof(rt.LogicOp));
			HashStreamUpdate(handle, &rt.WriteMask, sizeof(rt.WriteMask));
		}
	}
};



enum class DepthWriteMask
{
	Zero,
	All
};

enum class StencilOp
{
	Keep,
	Zero,
	Replace,
	IncrSat,
	DecrSat,
	Invert,
	Incr,
	Decr
};


struct DepthStencilDesc
{
	bool DepthEnable = false;
	DepthWriteMask DepthWriteMask = DepthWriteMask::All;
	ComparisonFunc DepthFunc = ComparisonFunc::Less;

	bool StencilEnable = false;
	u8 StencilReadMask = 0xff;
	u8 StencilWriteMask = 0xff;

	StencilOp FrontStencilFail = StencilOp::Keep;
	StencilOp FrontStencilDepthFail = StencilOp::Keep;
	StencilOp FrontStencilPass = StencilOp::Keep;
	ComparisonFunc FrontStencilFunc = ComparisonFunc::Always;

	StencilOp BackStencilFail = StencilOp::Keep;
	StencilOp BackStencilDepthFail = StencilOp::Keep;
	StencilOp BackStencilPass = StencilOp::Keep;
	ComparisonFunc BackStencilFunc = ComparisonFunc::Always;

	void HashUpdate(std::any handle) const
	{
		HashStreamUpdate(handle, &DepthEnable, sizeof(DepthEnable));
		HashStreamUpdate(handle, &DepthWriteMask, sizeof(DepthWriteMask));
		HashStreamUpdate(handle, &DepthFunc, sizeof(DepthFunc));
		HashStreamUpdate(handle, &StencilEnable, sizeof(StencilEnable));
		HashStreamUpdate(handle, &StencilReadMask, sizeof(StencilReadMask));
		HashStreamUpdate(handle, &StencilWriteMask, sizeof(StencilWriteMask));
		HashStreamUpdate(handle, &FrontStencilFail, sizeof(FrontStencilFail));
		HashStreamUpdate(handle, &FrontStencilDepthFail, sizeof(FrontStencilDepthFail));
		HashStreamUpdate(handle, &FrontStencilPass, sizeof(FrontStencilPass));
		HashStreamUpdate(handle, &FrontStencilFunc, sizeof(FrontStencilFunc));
		HashStreamUpdate(handle, &BackStencilFail, sizeof(BackStencilFail));
		HashStreamUpdate(handle, &BackStencilDepthFail, sizeof(BackStencilDepthFail));
		HashStreamUpdate(handle, &BackStencilPass, sizeof(BackStencilPass));
		HashStreamUpdate(handle, &BackStencilFunc, sizeof(BackStencilFunc));

	}
};

enum class ShaderBindType
{
	RootConstant = 0,
	RootCBV,
	RootSRV,
	RootUAV,
	TableCBV,
	TableSRV,
	TableUAV,
	TableSampler,
	Max
};

struct RootSignatureParamDesc
{
	ShaderBindType Type;
	u32 DescriptorNum;
	DescriptorAllocation Alloc;
};

class RootSignature
{
public:
	struct Desc
	{
		u32 RootConstant32Num;
		u32 RootCBVNum;
		u32 RootSRVNum;
		u32 RootUAVNum;
		u32 TableCBVNum;
		u32 TableSRVNum;
		u32 TableUAVNum;
		u32 TableSamplerNum;

		u64 HashResult() const
		{
			return Hash(this, sizeof(Desc));
		}
	};

	constexpr static u32 cDescriptorTableSpace = 0;
	constexpr static u32 cRootDescriptorSpace = 1;
	constexpr static u32 cRootConstantSpace = 2;

	virtual ~RootSignature()
	{
		for (u32 i = 0; i < _ParamsDesc.size(); i++)
		{
			if (!_ParamsDesc[i].Alloc.Heap)
				continue;

			_ParamsDesc[i].Alloc.Heap->FreeBlock(_ParamsDesc[i].Alloc);
		}
	};
	virtual std::any Handle() const { return nullptr; }

	const std::vector<RootSignatureParamDesc>& GetParamDesc() const
	{
		return _ParamsDesc;
	}

	const Desc& GetDesc() const
	{
		return _Info;
	}

	bool Satisfy(const RootSignature::Desc& desc)
	{
		if (desc.RootCBVNum > _Info.RootCBVNum)
			return false;
		if (desc.RootSRVNum > _Info.RootSRVNum)
			return false;
		if (desc.RootUAVNum > _Info.RootUAVNum)
			return false;

		if (desc.TableCBVNum > _Info.TableCBVNum)
			return false;
		if (desc.TableSRVNum > _Info.TableSRVNum)
			return false;
		if (desc.TableUAVNum > _Info.TableUAVNum)
			return false;
		if (desc.TableSamplerNum > _Info.TableSamplerNum)
			return false;

		return true;
	}
 
protected:
	Desc _Info;
	std::vector<RootSignatureParamDesc> _ParamsDesc;
};

struct ShaderParameter
{
	std::string Name;
	ShaderBindType BindType;
	u32 RootParamIndex;
	u32 TableOffset;
	u32 DescriptorNum;
	RenderResource* Resource = nullptr;
};

class RenderContext;

enum class PipelineType
{
	Graphic = 0,
	Compute
};

class RenderPipeline
{
public:
	RHI_API RootSignature* GetRootSignature() const
	{
		return _RootSignature;
	}

	const std::string& GetName() const
	{
		return _Name;
	}

	PipelineType GetType()
	{
		return _Type;
	}

	virtual std::any Handle() { return nullptr; }
	virtual ~RenderPipeline() {};

	RHI_API void CommitParameter(RenderContext* ctx);
	RHI_API void BindParameter(const std::string& name, RenderResource* resource);
	RHI_API void AllocateParameters(RootSignature* rs, std::array<ShaderResource*, (u32)ShaderProfile::MAX>& shaders);

protected:
	RootSignature* _RootSignature;
	std::string _Name;
	PipelineType _Type;

	std::unordered_map<std::string, ShaderParameter*> _ShaderParameters;
	// shader parameter table data.
	std::vector<ShaderParameter*> _CBVs;
	std::vector<ShaderParameter*> _SRVs;
	std::vector<ShaderParameter*> _UAVs;
	std::vector<ShaderParameter*> _Samplers;
};

class GraphicPipeline : public RenderPipeline
{
public:
	struct Desc
	{
		InputLayout    VertexLayout;
		ShaderCreateDesc VS;
		ShaderCreateDesc PS;
		BlendDesc       BlendState;
		u32             SampleMask = 0xffffffff;
		RasterizerDesc  RasterizerState;
		DepthStencilDesc DepthStencilState;
		PrimitiveTopology Topology = PrimitiveTopology::Triangle;
		std::vector<PixelFormat> RVTFormats = { PixelFormat::R8G8B8A8_UNORM };
		PixelFormat DSVFormat = PixelFormat::UNKNOWN;
		u32 SampleCount = 1;
		u32 SampleQuality = 0;

		void HashUpdate(std::any handle) const
		{
			VertexLayout.HashUpdate(handle);
			VS.HashUpdate(handle);
			PS.HashUpdate(handle);
			BlendState.HashUpdate(handle);
			HashStreamUpdate(handle, &SampleMask, sizeof(SampleMask));
			RasterizerState.HashUpdate(handle);
			DepthStencilState.HashUpdate(handle);
			HashStreamUpdate(handle, &Topology, sizeof(Topology));
			for (u32 i = 0; i < RVTFormats.size(); i++)
			{
				HashStreamUpdate(handle, &RVTFormats[i], sizeof(RVTFormats[i]));
			}
			HashStreamUpdate(handle, &DSVFormat, sizeof(DSVFormat));
			HashStreamUpdate(handle, &SampleCount, sizeof(SampleCount));
			HashStreamUpdate(handle, &SampleQuality, sizeof(SampleQuality));
		}

		u64 HashResult() const
		{
			std::any handle = HashStreamStart();
			HashUpdate(handle);
			return HashStreamEnd(handle);
		}
	};
 
	Desc  Info;
 
};

class ComputePipeline : public RenderPipeline
{
public:
	struct Desc
	{
		ShaderCreateDesc CS;

		u64 HashResult() const
		{
			return CS.HashResult();
		}
	};

	Desc  Info;
 
};



