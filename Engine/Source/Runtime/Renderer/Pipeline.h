#pragma once

class RootSignature
{
public:
	virtual ~RootSignature() {}
	virtual std::any Handle() { return nullptr; }

};

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
	Triangle,
	Patch
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
	bool DepthEnable = true;
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
};


class GraphicPipeline
{
public:
	struct Desc
	{
		std::vector<InputLayoutDesc>  InputLayout;
		RootSignature* RootSignature;
		ShaderObject* VertexShader;
		ShaderObject* PixelShader;
		BlendDesc       BlendState;
		u32             SampleMask = 0xffffffff;
		RasterizerDesc  RasterizerState;
		DepthStencilDesc DepthStencilState;
		PrimitiveTopology Topology = PrimitiveTopology::Triangle;
		std::vector<PixelFormat> RVTFormats = { PixelFormat::R8G8B8A8_UNORM };
		PixelFormat DSVFormat = PixelFormat::D32_FLOAT_S8X24_UINT;
		u32 SampleCount = 1;
		u32 SampleQuality = 0;
	};

	virtual ~GraphicPipeline() {}
};