#include "Private.h"

Material::~Material()
{
	for (auto shader : _Shaders)
	{
		if (shader)
			delete shader;
	}
}

Material* Material::Create(const std::array<ShaderObject*, (size_t)ShaderProfile::MAX>& shaders)
{
	Material* mat = new Material;
	mat->_Shaders = shaders;
	return mat;
}