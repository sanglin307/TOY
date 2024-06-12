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

	void HashUpdate(XXH64_state_t* state) const
	{
		for (u32 i = 0; i < Desc.size(); i++)
		{
			const InputLayoutDesc& desc = Desc[i];
			XXH64_update(state, desc.SemanticName.c_str(), desc.SemanticName.length());
			XXH64_update(state, &desc.SemanticIndex, sizeof(desc.SemanticIndex));
			XXH64_update(state, &desc.Format, sizeof(desc.Format));
			XXH64_update(state, &desc.SlotIndex, sizeof(desc.SlotIndex));
			XXH64_update(state, &desc.SlotOffset, sizeof(desc.SlotOffset));
			XXH64_update(state, &desc.SlotClass, sizeof(desc.SlotClass));
			XXH64_update(state, &desc.InstanceStepRate, sizeof(desc.InstanceStepRate));
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

	void HashUpdate(XXH64_state_t* state) const
	{
		XXH64_update(state, &Fill, sizeof(Fill));
		XXH64_update(state, &Cull, sizeof(Cull));
		XXH64_update(state, &FrontCounterClockwise, sizeof(FrontCounterClockwise));
		XXH64_update(state, &DepthClip, sizeof(DepthClip));
		XXH64_update(state, &Multisample, sizeof(Multisample));
		XXH64_update(state, &LineAA, sizeof(LineAA));
		XXH64_update(state, &ConservativeMode, sizeof(ConservativeMode));
		XXH64_update(state, &DepthBias, sizeof(DepthBias));
		XXH64_update(state, &DepthBiasClamp, sizeof(DepthBiasClamp));
		XXH64_update(state, &SlopScaledDepthBias, sizeof(SlopScaledDepthBias));
		XXH64_update(state, &SampleCount, sizeof(SampleCount));
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

	void HashUpdate(XXH64_state_t* state) const
	{
		XXH64_update(state, &AlphaToCoverageEnable, sizeof(AlphaToCoverageEnable));
		XXH64_update(state, &IndependentBlendEnable, sizeof(IndependentBlendEnable));
		for (u32 i = 0; i < MaxRenderTargetNumber; i++)
		{
			const BlendDesc::RenderTarget& rt = RenderTargets[i];
			XXH64_update(state, &rt.Enable, sizeof(rt.Enable));
			XXH64_update(state, &rt.LogicOpEnable, sizeof(rt.LogicOpEnable));
			XXH64_update(state, &rt.Src, sizeof(rt.Src));
			XXH64_update(state, &rt.Dest, sizeof(rt.Dest));
			XXH64_update(state, &rt.Op, sizeof(rt.Op));
			XXH64_update(state, &rt.SrcAlpha, sizeof(rt.SrcAlpha));
			XXH64_update(state, &rt.DestAlpha, sizeof(rt.DestAlpha));
			XXH64_update(state, &rt.AlphaOp, sizeof(rt.AlphaOp));
			XXH64_update(state, &rt.LogicOp, sizeof(rt.LogicOp));
			XXH64_update(state, &rt.WriteMask, sizeof(rt.WriteMask));
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

	void HashUpdate(XXH64_state_t* state) const
	{
		XXH64_update(state, &DepthEnable, sizeof(DepthEnable));
		XXH64_update(state, &DepthWriteMask, sizeof(DepthWriteMask));
		XXH64_update(state, &DepthFunc, sizeof(DepthFunc));
		XXH64_update(state, &StencilEnable, sizeof(StencilEnable));
		XXH64_update(state, &StencilReadMask, sizeof(StencilReadMask));
		XXH64_update(state, &StencilWriteMask, sizeof(StencilWriteMask));
		XXH64_update(state, &FrontStencilFail, sizeof(FrontStencilFail));
		XXH64_update(state, &FrontStencilDepthFail, sizeof(FrontStencilDepthFail));
		XXH64_update(state, &FrontStencilPass, sizeof(FrontStencilPass));
		XXH64_update(state, &FrontStencilFunc, sizeof(FrontStencilFunc));
		XXH64_update(state, &BackStencilFail, sizeof(BackStencilFail));
		XXH64_update(state, &BackStencilDepthFail, sizeof(BackStencilDepthFail));
		XXH64_update(state, &BackStencilPass, sizeof(BackStencilPass));
		XXH64_update(state, &BackStencilFunc, sizeof(BackStencilFunc));

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

		void HashUpdate(XXH64_state_t* state) const
		{
			XXH64_update(state, Name.c_str(), Name.length());
			VertexLayout.HashUpdate(state);
			VS.HashUpdate(state);
			PS.HashUpdate(state);
			BlendState.HashUpdate(state);
			XXH64_update(state, &SampleMask, sizeof(SampleMask));
			RasterizerState.HashUpdate(state);
			DepthStencilState.HashUpdate(state);
			XXH64_update(state, &Topology, sizeof(Topology));
			for (u32 i = 0; i < RVTFormats.size(); i++)
			{
				XXH64_update(state, &RVTFormats[i], sizeof(RVTFormats[i]));
			}
			XXH64_update(state, &DSVFormat, sizeof(DSVFormat));
			XXH64_update(state, &SampleCount, sizeof(SampleCount));
			XXH64_update(state, &SampleQuality, sizeof(SampleQuality));
		}

		u64 HashResult() const
		{
			XXH64_state_t* const state = XXH64_createState();
			XXH64_reset(state, 0);
			HashUpdate(state);
			XXH64_hash_t const hash = XXH64_digest(state);
			XXH64_freeState(state);
			return hash;
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



