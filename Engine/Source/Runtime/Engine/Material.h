#pragma once

class Material
{
public:
	static Material* Create(const std::array<ShaderObject*, (size_t)ShaderProfile::MAX>& shaders);
	virtual ~Material();
private:
	std::array<ShaderObject*, (size_t)ShaderProfile::MAX> _Shaders;
};