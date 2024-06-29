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
	RenderTexture* Texture;
};

struct Material
{
	std::string             Name;
    bool                    Unit;
    bool                    DoubleSide;

	glm::vec4               BaseColorFactor;
	glm::vec3               EmissiveFactor;
	float                   MetalnessFactor;
	float                   RoughnessFactor;
    float                   IOR;

	MaterialTexture          BaseColorTexture;
	MaterialTexture          NormalTexture;
	MaterialTexture          RoughnessMetalnessTexture;
	MaterialTexture          EmissiveTexture;

    float                   AnisotropyStrength = 0.0f;
    float                   AnisotropyRotation = 0.0f;
	MaterialTexture          AnisotropyTexture;

	MaterialBlend           BlendMode;
    float                   AlphaCutoff;
};