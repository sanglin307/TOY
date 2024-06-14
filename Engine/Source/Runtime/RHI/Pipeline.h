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

enum class ComparisonFunc
{
	None,
	Never,
	Less,
	Equal,
	LessEqual,
	Greater,
	NotEqual,
	GreaterEqual,
	Always
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
	f32 DepthBiasClamp =0.f;
	f32 SlopScaledDepthBias = 0.f;
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
	RootCBV = 0,
	RootSRV,
	RootUAV,
	TableOneCBV,
	TableOneSRV,
	TableOneUAV,
	TableOneSampler,
	TableMultipleCBV,
	TableMultipleSRV,
	TableMultipleUAV,
	TableMultipleSampler,
	Max
};

struct ShaderParameter
{
	std::string Name;
	ShaderBindType BindType;
	u32 RootParamIndex; 
	RenderResource* Resource;
};

class RootSignature
{
public:
	virtual ~RootSignature() {};
	virtual std::any Handle() { return nullptr; }

	virtual ShaderParameter* Allocate(const SRBoundResource& res) = 0;

protected:
	std::unordered_map<std::string, ShaderParameter*> _ParamsMap;
};

class RenderContext;
class GraphicPipeline
{
public:
	struct Desc
	{
		std::string Name;
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
			HashStreamUpdate(handle, Name.c_str(), Name.length());
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

	virtual ~GraphicPipeline() {}
	virtual std::any Handle() { return nullptr; }

	RHI_API void BindParameter(RenderContext* ctx);
	RHI_API void BindParameter(const std::string& name, RenderResource* resource);
	RHI_API void AddParameter(ShaderParameter* parameter);
	RHI_API ShaderParameter* GetParameter(const std::string& name);

	Desc  Info;

protected:
	std::unordered_map<std::string, ShaderParameter*> _ShaderParameters;
};



