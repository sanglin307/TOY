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
	glm::vec3   _BaseColor;
	glm::vec3   _Emissive;
	float          _Metalness;
	float          _Roughness;
	RenderTexture* _DiffuseTexture;
	RenderTexture* _NormalTexture;
	RenderTexture* _RoughnessMetalnessTexture;
	RenderTexture* _EmissiveTexture;
	MaterialBlend  _BlendMode;
};