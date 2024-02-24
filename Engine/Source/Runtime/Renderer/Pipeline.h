#pragma once

#include "Defines.h"

class RootSignature
{
public:
	virtual ~RootSignature() {}
	virtual std::any Handle() { return nullptr; }

};

class InputLayout
{
public:
	enum class SlotClass
	{
		PerVertex = 0,
		PerInstance
	};

	struct Desc
	{
		std::string Name;
		u32 Index;
		PixelFormat Format;
		u32 SlotIndex;
		u32 AlignedByteOffset;
		SlotClass SlotType;
		u32 InstanceStepRate;
	};

private:
	std::vector<Desc> _Desc;

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
	Reverse,
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

struct RasterizerDesc
{
	FillMode Fill;
	CullMode Cull;
	bool FrontCounterClockwise;
	bool DepthClip;
	bool Multisample;
	bool LineAA;
	bool ConservativeMode;
	i32 DepthBias;
	f32 DepthBiasClamp;
	f32 SlopScaledDepthBias;
	u32 SampleCount;
};

constexpr static u32 MaxRenderTargetNumber = 8;

struct BlendDesc
{
	struct RenderTarget
	{
		bool Enable;
		bool LogicOpEnable;
		BlendFactor Src;
		BlendFactor Dest;
		BlendOp Op;
		BlendFactor SrcAlpha;
		BlendFactor DestAlpha;
		BlendOp AlphaOp;
		LogicOp Logic;
		u8 WriteMask;

	};
	bool AlphaToCoverageEnable;
	bool IndependentBlendEnable;
	RenderTarget RenderTargets[MaxRenderTargetNumber];
};

class GraphicPipeline
{
public:

private:
	InputLayout* _InputLayout;
	RootSignature* _RootSignature;
	std::vector<u8> _VertexShader;
	std::vector<u8> _PixelShader;
	std::vector<u8> _HullShader;
	std::vector<u8> _DomainShader;
	std::vector<u8> _GeometryShader;
	BlendDesc _BlendState;
	u32 _SampleMask;
	RasterizerDesc _RasterizerState;

};