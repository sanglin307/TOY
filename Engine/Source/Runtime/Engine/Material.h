#pragma once


enum class MaterialBlend
{
	Opaque,
	Masked,
	Transparent,
};

class Material
{
public:


private:
	hlslpp::float3 _BaseColor;
	hlslpp::float3 _Emissive;
	float          _Metalness;
	float          _Roughness;
	RenderTexture* _DiffuseTexture;
	RenderTexture* _NormalTexture;
	RenderTexture* _RoughnessMetalnessTexture;
	RenderTexture* _EmissiveTexture;
	MaterialBlend  _BlendMode;
};