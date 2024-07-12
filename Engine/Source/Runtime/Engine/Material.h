#pragma once


enum class MaterialBlend
{
	Opaque,
	Masked,
	Transparent,
};

struct MaterialTexture
{
	u32 TextureCoordIndex = 0;
	RenderTexture* Texture = nullptr;
};

struct Material
{
	std::string             Name;
    bool                    Unit;
    bool                    DoubleSide;

	float4                  BaseColorFactor;
	float3                  EmissiveFactor;
	float                   MetalnessFactor;
	float                   RoughnessFactor;
    float                   IOR;

	MaterialTexture          BaseColorTexture;
	MaterialTexture          NormalTexture;
	MaterialTexture          RoughnessMetalnessTexture;
	MaterialTexture          EmissiveTexture;

	MaterialTexture          AnisotropyTexture;
    float                   AnisotropyStrength = 0.0f;
    float                   AnisotropyRotation = 0.0f;
	

	MaterialBlend           BlendMode;
    float                   AlphaCutoff;
};