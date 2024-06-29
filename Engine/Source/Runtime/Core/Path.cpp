#include "Private.h"

static std::filesystem::path bin_path = std::filesystem::current_path();
static std::filesystem::path log_path = bin_path / "Logs";
static std::filesystem::path config_path = bin_path.parent_path().parent_path() / "Engine" / "Config";
static std::filesystem::path shader_path = bin_path.parent_path().parent_path() / "Engine" / "Shaders";
static std::filesystem::path shader_out_path = bin_path.parent_path().parent_path() / "Engine" / "Shaders" / "Output";
static std::filesystem::path pso_path = bin_path.parent_path().parent_path() / "Engine" / "Shaders" / "PSO";
static std::filesystem::path images_path = bin_path.parent_path().parent_path() / "Engine" / "Assets" / "Images";
static std::filesystem::path glTFs_path = bin_path.parent_path().parent_path() / "Engine" / "Assets" / "glTFs";

const std::filesystem::path& PathUtil::Bin()
{
	return bin_path;
}

const std::filesystem::path& PathUtil::Logs()
{
	return log_path;
}

const std::filesystem::path& PathUtil::Config()
{
	return config_path;
}

const std::filesystem::path& PathUtil::Shaders()
{
	return shader_path;
}

const std::filesystem::path& PathUtil::ShaderOutput()
{
	return shader_out_path;
}

const std::filesystem::path& PathUtil::PSO()
{
	return pso_path;
}

const std::filesystem::path& PathUtil::Images()
{
	return images_path;
}

const std::filesystem::path& PathUtil::glTFs()
{
	return glTFs_path;
}